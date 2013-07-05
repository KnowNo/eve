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

#include "exceptions.h"
#include <fstream>

/** \addtogroup Lib
  * @{
  */

/** Returns an integer with only @p n -th bit enabled. */
#define eve_bit(n) (1 << n)

namespace eve
{

template <typename T>
class flagset
{
public:
  flagset() : m_flags(0) { }
  flagset(T flag): m_flags(type(flag)) { }
  bool isset(T flag) const { return (m_flags & type(flag)) != 0; }
  void set(T flag) { m_flags |= type(flag); }
  void set(T flag, bool value) { if (value) this->set(flag); else reset(flag); }
  void reset(T flag) { m_flags &= ~type(flag); }
  void reset() { m_flags = 0; }
  flagset operator|(T flag)
  {
    return flagset(type(flag) | m_flags);
  }
  flagset operator|(flagset rhs)
  {
    return flagset(rhs.m_flags | m_flags);
  }

private:
  typedef typename std::conditional<std::is_enum<T>::value,
    typename std::underlying_type<T>::type,
    T
  >::type type;
  flagset(type flags) : m_flags(flags) { }
  type m_flags;
};

/** Tries to open an ifstream at @p path and checks that it's good.
    @note If not good throws an eve::file_not_found_error. */
std::ifstream open_fstream(const std::string& path);

} // eve

/** }@ */

