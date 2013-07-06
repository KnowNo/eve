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
#include <iostream>
#include <cstdint>

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

#ifndef EVE_RELEASE

struct allocation
{
  const char* function;
  const char* file;
  unsigned line;
  const void* ptr;
  size_t size;
  bool is_array;
  eve::size place;
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
  s_initialized = false;

}

} // eve

static void check_initialization()
{
  if (!s_initialized)
    throw std::runtime_error("Memory debugger not initialized. Have you sure you're using an eve::application?");
}

void eve::memory_debugger::track(const void* ptr, bool inplace)
{
  check_initialization();

  std::lock_guard<std::mutex> lock(s_mutex);

  if (!s_enabled)
    return;

  auto it = s_allocations.find(ptr);
  if (it == s_allocations.end())
    s_allocations.insert(it, std::make_pair(ptr, allocation(file, line, function, ptr, size, isArray, (Allocation::Place)inPlace)));
  else
  {
    if (it->second.place != Allocation::DYNAMIC || !inPlace)
      reportError(file, line, function, ptr, format("multiple allocation_t in line %;", line));
    it->second.place = Allocation::BOTH;
  }
}

void eve::memory_debugger::untrack(const void* ptr, bool inplace)
{

}

void eve::memory_debugger::transfer(const void* from, const void* to)
{

}

#endif // EVE_RELEASE