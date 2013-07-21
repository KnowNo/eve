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
#include <ostream>

/**
 * \addtogroup Lib
 * @{
 */

namespace eve {

class string
{
public:
  string();
  string(const char* data);
  string(const string& rhs);
  string(string&& rhs);
  ~string();

  /** @returns the number of C-char characters composing this string. */
  eve::size size() const { return m_size & ~eve::size_msb; }

  /** @returns a pointer to an array that contains a null-terminated sequence 
   ** of characters (i.e., a C-string) representing the current value of the string object. */
  const char* data() const;

  /** Sets this string to "". */
  void clear();

  /** @returns true when the two strings have the same value. */
  bool operator==(const string& rhs) const;
  
  /** @returns true when the two strings have the same value. */
  bool operator==(const char* rhs) const;

  string& operator=(const char* rhs);
  string& operator=(const string& rhs);
  string& operator=(string&& rhs);

  string operator+(const char* rhs);
  string operator+(const string& rhs) { return append(rhs.data(), rhs.size()); }

private:
  string(const char* data, eve::size size);
  bool is_reference() const
  {
    return (m_size & eve::size_msb) != 0;
  }
  string append(const char* rhs, eve::size rhssize);

  const void* m_data;
  eve::size m_size;

  friend class stringbuf;
};

inline bool operator==(const char* lhs, const string& str)
{
  return str == lhs;
}

inline std::ostream& operator<<(std::ostream& os, const string& str)
{
  os << str.data();
  return os;
}

} // namespace eve

/** }@ */
