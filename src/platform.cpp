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

#include "eve/platform.h"
#include "eve/exceptions.h"
#include <atomic>

eve::size eve::arithmetic_type_size(arithmetic_type type)
{
  static const size k_sizes[] =
  {
    eve_sizeof(char), eve_sizeof(unsigned char), 
    eve_sizeof(short), eve_sizeof(unsigned short), 
    eve_sizeof(int), eve_sizeof(unsigned int), 
    eve_sizeof(long long), eve_sizeof(unsigned long long), 
    eve_sizeof(float), eve_sizeof(double) 
  };
  return k_sizes[size(type)];
}

static std::atomic<eve::id> s_counter(0);

eve::id eve::unique_id()
{
  return ++s_counter;
}

static bool s_initialized = false;

#ifdef _MSC_VER

#include <Windows.h>
#include <Dbghelp.h>

static eve::uint32 s_process_handle;

namespace eve
{
  void initialize_platform()
  {
  #ifndef EVE_RELEASE
    HANDLE process = GetCurrentProcess();

    SymSetOptions(SYMOPT_LOAD_LINES);

    // initialize process symbols
    if (!SymInitialize(process, NULL, TRUE))
    {
      int err = GetLastError();
      eve::system_error("Could not load process symbols.", err);
    }
  #endif
    s_initialized = true;
  }
}

#else
#error Implement platform specific functions.
#endif