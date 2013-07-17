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

#include "platform.h"
#include "storage.h"
#include <ostream>

/** \addtogroup Lib
  * @{
  */

namespace eve {

class callstack
{
public:
  static const size k_max_trace_size = 8;

  class symbol
  {
  public:
    symbol();
    
    /** @returns the name of the function. */
    const char* function() const { return m_function; }

    /** @returns the filename of the source file where the frame function is called. */
    const char* file() const { return m_file; }

    /** @returns the line in the source file where the frame function is called. */
    eve::size line() const { return m_line; }

  private:
    static const size k_max_function_length = 64;
    char m_function[k_max_function_length];
    const char* m_file;
    eve::size m_line;
    friend class callstack;
  };

  /** Constructs a callstack. If @p capture is true, it will capture the
      stack trace on creation.
      @see capture() */
  callstack(bool capture = true, eve::size skipframes = 0);

  /** Captures the current stack trace.
      @param skipframes Is the number of frames to skip before from the top.
      @note capture() itself is skipped. */
  void capture(eve::size skipframes = 0);

  /** @returns the number of captured frames. */
  eve::size size() const { return m_size; }

  /** @returns a hash number. Identical stack frames have equal hash number. */
  uint32 hash() const { return m_hash; }

  /** Resolves the trace with index @p index and returns the corresponding symbol. */
  symbol fetch(eve::size index) const;

  //// Operators ////
  bool operator==(const callstack& rhs) const { return m_hash == rhs.m_hash; }
  bool operator<(const callstack& rhs) const { return m_hash < rhs.m_hash; }

private:
  uint32 m_hash;
  void* m_trace[k_max_trace_size];
  uint16 m_size;
};

} // eve

std::ostream& operator<<(std::ostream& output, const eve::callstack& callstack);

/** }@ */
