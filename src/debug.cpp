/******************************************************************************\
* This source file is part of the 'eve' framework.                             *
* (A linear, elegant, modular engine for rapid game development)               *
*                                                                              *
* The MIT License (MIT)                                                        *
*                                                                              *
* Copyright (c) 2013                                                           *
*                                                                              *
* Permission is hereby granted, free of charge, to any person obtaining a copy *
* of this software and associated documentation files (the "Software"), to deal*
* in the Software without restriction, including without limitation the rights *
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell    *
* copies of the Software, and to permit persons to whom the Software is        *
* furnished to do so, subject to the following conditions:                     *
*                                                                              *
* The above copyright notice and this permission notice shall be included in   *
* all copies or substantial portions of the Software.                          *
*                                                                              *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE  *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,*
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN    *
* THE SOFTWARE.                                                                *
\******************************************************************************/

#include "eve/debug.h"
#include <string>
#include <iostream>
#include <cstdint>
#include <fstream>

#ifndef EVE_RELEASE
#  include <mutex>
#  include <unordered_map>
#endif

#ifdef EVE_WINDOWS
#  include <Windows.h>
#endif

void eve::show_error(const char* message)
{
#ifdef EVE_WINDOWS
  MessageBox(NULL, message, "Error", MB_ICONERROR | MB_OK);
#else
  std::cerr << "Error: " << message;
#endif
}

void eve::abort(const char* message)
{
  eve::show_error(message);

#ifndef EVE_RELEASE
  struct uncatchable_exception {};
  throw uncatchable_exception();
#else
  ::abort();
#endif // EVE_RELEASE
}

// MEMORY DEBUGGER /////////////////////////////////////////////////////////////////////////////////

eve::memory_error::memory_error(const void* ptr, const std::string& error) throw()
  : std::logic_error(error), m_ptr(ptr)
{
}

#ifndef EVE_RELEASE

struct allocation
{
  const char* function;
  const char* file;
  unsigned line;
  const void* ptr;
  bool inheap;
  eve::size allocations;
};

static bool s_initialized = false;
static bool s_enabled = false;
static std::mutex s_mutex;
static std::unordered_map<const void*, allocation> s_allocations;

namespace eve {

void initialize_memory_debugger(bool enabled)
{
  s_enabled = enabled;
  s_initialized = true;
}

void terminate_memory_debugger()
{
  if (s_initialized && s_enabled)
  {
    std::lock_guard<std::mutex> lock(s_mutex);

    std::ofstream ofs("eve_memory_debugger_report.txt");
  
    ofs << "Eve Memory Debugger Report\n=======================\n";

    // Find all not-freed allocation_ts.
    if (s_allocations.empty())
      ofs << " -  No leaks found, nice job.\n";

    for (auto& alloc: s_allocations)
      ofs << "  - " << "In function " << alloc.second.function
                    << " in file " << alloc.second.file
                    << " at line:\n      " << alloc.second.line
                    << " ptr " << alloc.second.ptr
                    << " in heap: " << alloc.second.inheap
                    << " allocations: " << alloc.second.allocations;
  }

  s_initialized = false;
  s_enabled = false;
  s_allocations.clear();
}

} // eve

static void check_initialization()
{
  if (!s_initialized)
    throw std::runtime_error("Memory debugger not initialized. Have you sure you're using an eve::application?");
}

static void report_error(const void* ptr, const std::string& message)
{
  throw eve::memory_error(ptr, message);
}

static void report_error(const allocation& alloc, const void* ptr, const std::string& message)
{
  std::string msg = message;
  msg += "\nAllocation performed in function ";
  msg += alloc.function;
  msg += " in file ";
  msg += alloc.file;
  msg += " at line ";
  msg += std::to_string(alloc.line);
  msg += ".";
  report_error(ptr, msg);
}

void eve::memory_debugger::track(eve_source_location_args, const void* ptr, bool inplace)
{
  check_initialization();

  if (!s_enabled)
    return;

  std::lock_guard<std::mutex> lock(s_mutex);

  auto it = s_allocations.find(ptr);
  if (it == s_allocations.end())
  {
    allocation alloc = {function, file, line, ptr, !inplace, 1};
    s_allocations.insert(it, std::make_pair(ptr, alloc));
  } else
  {
    if (!inplace)
      report_error(ptr, "Not in-place allocation in already allocated memory.");
    ++it->second.allocations;
  }
}

void eve::memory_debugger::untrack(eve_source_location_args, const void* ptr, bool inplace)
{
  check_initialization();

  if (!s_enabled)
    return;

  std::lock_guard<std::mutex> lock(s_mutex);
  
  auto it = s_allocations.find(ptr);
  if (it == s_allocations.end())
    report_error(ptr, "Trying to delete an untracked pointer.");
  else
  {
    bool error = false;

    // Last allocation, make sure inplace creation/deletion match.
    if (it->second.allocations == 1)
    {
      // In place deletion on a a non-inplace allocation (an heap allocation). Error.
      if (inplace && it->second.inheap)
        report_error(it->second, ptr, "In-place deletion as the last deletion of a non in-place allocation.");

      // Heap deletion of an allocation that started as an in-place allocation (e.g. on a stack allocated buffer).
      if (!inplace && !it->second.inheap)
        report_error(ptr, "Not in-place deletion on an originally in-place allocation.");
    }
    else if (it->second.allocations == 2)
    {
      // Heap deletion of an allocation that contains an in-place allocation. The in place allocation should be
      // deleted first.
      if (!inplace && it->second.inheap)
        report_error(ptr, "Not in-place deletion an allocation that contains an in-place allocation. The in place allocation should be deleted first.");
    }

    // All fine, decrement the number of allocations.
    --it->second.allocations;
    if (it->second.allocations == 0)
      s_allocations.erase(it);
  }
}

void eve::memory_debugger::transfer(eve_source_location_args, const void* from, const void* to)
{
  check_initialization();

  if (!s_enabled)
    return;

  std::lock_guard<std::mutex> lock(s_mutex);

  auto it = s_allocations.find(from);
  if (it == s_allocations.end())
    return;

  if (it->second.inheap && it->second.allocations == 1)
    report_error(it->second, from, "Trying to transfer the in-place allocation of a non in-place allocation.");

  auto alloc = it->second;
  alloc.function = function;
  alloc.file = file;
  alloc.line = line;
  alloc.ptr = to;
  s_allocations.erase(it);
  s_allocations.insert(std::make_pair(to, alloc));
}

#endif // EVE_RELEASE