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

/** \addtogroup Lib
  * @{
  */

namespace eve {

template<class T>
class range
{
public:
  range() { }
  range(const T& begin, const T& end)
    : m_begin(begin), m_end(end)
  { }

  T begin() const { return m_begin; }
  T end() const { return m_end; }

private:
  T m_begin, m_end;
};

////////////////////////////////////////////////////////////////////////////////

template<class T>
class range<T*>
{
public:
  range() : m_begin(nullptr), m_end(nullptr) { }
  range(T* begin, T* end)
    : m_begin(begin), m_end(end)
  { }

  T* begin() const { return m_begin; }
  T* end() const { return m_end; }

private:
  T* m_begin;
  T* m_end;
};

} // eve

/** }@ */
