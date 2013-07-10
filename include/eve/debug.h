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

#pragma once

#include "macro.h"
#include "platform.h"
#include <stdexcept>

#define eve_source_location_args const char* function, const char* file, unsigned line
#define eve_here __FUNCTION__, __FILE__, __LINE__
#define eve_forward_source_location_args function, file, line

#ifndef EVE_RELEASE

/** A string containing file and line. */
#define eve_str_source_location "in function " eve_function_name " in file " __FILE__ " at line " eve_pp_stringify(__LINE__)

#define eve_assert(condition) if (!(condition)) eve::abort("Assertion fail (" #condition ") in " eve_str_source_location ".")
#define eve_debug_code(...) __VA_ARGS__

#else

#define eve_assert(condition) {}
#define eve_debug_code(...)

#endif

#define eve_internal_error eve::abort(("Internal error " eve_str_source_location "."))

/** \addtogroup Lib
  * @{
  */

namespace eve
{

/** Shows a fatal error message. */
void show_error(const char* message);

/** Shows a fatal error message and aborts execution. */
void abort(const char* message);

////////////////////////////////////////////////////////////////////////////////////////////////////

/* Define a macro that is empty body '{ }' when compiling in release mode. */
#ifndef EVE_RELEASE
#  define __eve_debug_empty_func ;
#else
#  define __eve_debug_empty_func {}
#endif // EVE_RELEASE

class memory_error : public std::logic_error
{
public:
  memory_error(const void* ptr, const std::string& error) throw();
  const void* ptr() const { return m_ptr; }
private:
  const void* m_ptr;
};

class memory_debugger
{
public:
  static void track(eve_source_location_args, const void* ptr, bool inplace) __eve_debug_empty_func
  static void untrack(eve_source_location_args, const void* ptr, bool inplace) __eve_debug_empty_func
  static void transfer(eve_source_location_args, const void* from, const void* to) __eve_debug_empty_func
};

} // eve

/** }@ */
