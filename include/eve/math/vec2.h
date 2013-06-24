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

#include "common.h"
#include "../platform.h"

/** \addtogroup Math
  * @{
  */

namespace eve {

// Forward declarations
template <typename> struct tvec2;

namespace math {

template <typename T>
struct tref2
{
  tref2(T& x, T& y);
  tref2(const tref2&);

  tref2& operator=(const tref2&);
  template <typename U>
  tref2& operator=(const tvec2<U>&);

  T& x;
  T& y;
};

} // math

template <typename T>
struct tvec2
{
  union
  {
    struct { T x, y; };
    struct { T w, h; };
  };

  tvec2();
  template <typename U>
  explicit tvec2(const U& x);
  template <typename U, typename V>
  explicit tvec2(const U& x, const V& y);
  template <typename U>
  explicit tvec2(const tvec2<U>& v);

  // This macro will generate all possible swizzle combinations of x and y.
  __eve_gen_swizzle2(x, y)
  
  T& operator[](eve::size index) { return (&x)[index]; }
  const T& operator[](eve::size index) const { return (&x)[index]; }

  template <typename U> tvec2<T>& operator+=(const U& v);
  template <typename U> tvec2<T>& operator+=(const tvec2<U>& v);
  template <typename U> tvec2<T>& operator-=(const U& v);
  template <typename U> tvec2<T>& operator-=(const tvec2<U>& v);
  template <typename U> tvec2<T>& operator*=(const U& v);
  template <typename U> tvec2<T>& operator*=(const tvec2<U>& v);
  template <typename U> tvec2<T>& operator/=(const U& v);
  template <typename U> tvec2<T>& operator/=(const tvec2<U>& v);
  template <typename U> tvec2<T>& operator=(const tvec2<U>& v);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef tvec2<real> vec2;
typedef tvec2<int16> vec2s;
typedef tvec2<uint16> vec2us;
typedef tvec2<int32> vec2i;
typedef tvec2<uint32> vec2u;
typedef tvec2<float> vec2f;
typedef tvec2<double> vec2d;

} // eve

/** @} */

#include "vec2.inl"
