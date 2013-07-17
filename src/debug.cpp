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
#include "eve/callstack.h"
#include <string>
#include <iostream>
#include <cstdint>
#include <fstream>
#include <sstream>

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
#ifdef _MSC_VER
  DebugBreak();
#else
#  error Implement debug break on this system.
#endif
#else
  ::abort();
#endif // EVE_RELEASE
}

// MEMORY DEBUGGER /////////////////////////////////////////////////////////////////////////////////

#ifndef EVE_RELEASE

struct allocation
{
  eve::callstack callstack;
  const void* ptr;
  bool inheap;
  eve::size allocations;
};

static bool s_enabled = false;
static std::mutex s_mutex;
static std::unordered_map<const void*, allocation> s_allocations;

namespace eve {

void initialize_memory_debugger(bool enabled)
{
  s_enabled = enabled;
}

void terminate_memory_debugger()
{
  if (s_enabled)
  {
    std::lock_guard<std::mutex> lock(s_mutex);

    std::ofstream ofs("eve_memory_debugger_report.txt");
  
    ofs << "Eve Memory Debugger Report\n=======================\n";

    // Find all not-freed allocation_ts.
    if (s_allocations.empty())
      ofs << " -  No leaks found, nice job.\n";

    for (auto& alloc: s_allocations)
    {
      ofs << "  - " << " Address " << alloc.second.ptr
                    << " in heap: " << alloc.second.inheap
                    << " allocations: " << alloc.second.allocations
                    << ".\n Callstack: \n";

      ofs << alloc.second.callstack;
    }

    if (s_allocations.size() > 0)
      eve::show_error("Memory debugger: leaks detected. See eve_memory_debugger_report.txt for more info.");
  }

  s_enabled = false;
  s_allocations.clear();
}

} // eve

static void report_error(const void* ptr, const std::string& message)
{
  eve::abort(message.c_str());
}

static void report_error(const allocation& alloc, const void* ptr, const std::string& message)
{
  std::stringstream ss;
  ss << message;
  ss << "\nCallstack:\n";
  ss << alloc.callstack;
  eve::abort(ss.str().c_str());
}

void eve::memory_debugger::track(const void* ptr, bool inplace)
{
  if (!s_enabled)
    return;
  
  std::lock_guard<std::mutex> lock(s_mutex);

  auto it = s_allocations.find(ptr);
  if (it == s_allocations.end())
  {
    allocation alloc = {eve::callstack(true, 1), ptr, !inplace, 1};
    s_allocations.insert(it, std::make_pair(ptr, alloc));
  } else
  {
    if (!inplace)
      report_error(ptr, "Not in-place allocation in already allocated memory.");
    ++it->second.allocations;
  }
}

void eve::memory_debugger::untrack(const void* ptr, bool inplace)
{
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

void eve::memory_debugger::transfer(const void* from, const void* to)
{
  if (!s_enabled)
    return;

  std::lock_guard<std::mutex> lock(s_mutex);

  auto it = s_allocations.find(from);
  if (it == s_allocations.end())
    return;

  if (it->second.inheap && it->second.allocations == 1)
    report_error(it->second, from, "Trying to transfer the in-place allocation of a non in-place allocation.");

  auto alloc = it->second;
  alloc.callstack.capture(1);
  alloc.ptr = to;
  s_allocations.erase(it);
  s_allocations.insert(std::make_pair(to, alloc));
}

#endif // EVE_RELEASE