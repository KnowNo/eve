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
#include "swizzle.h"

/** \addtogroup Math
  * @{
  */

namespace eve {

// Forward declarations
template <typename> struct tvec3;

namespace math {

template <typename T>
struct tref3
{
  tref3(T& x, T& y, T& z);
  tref3(const tref3&);

  tref3& operator=(const tref3&);
  template <typename U>
  tref3& operator=(const tvec3<U>&);

  T& x;
  T& y;
  T& z;
};

} // math

template <typename T>
struct tvec3
{
  union
  {
    struct { T x, y, z; };
    struct { T r, g, b; };
  };

  tvec3();
  template <typename U>
  explicit tvec3(const U& x);
  template <typename U, typename V, typename Z>
  explicit tvec3(const U& x, const V& y, const Z& z);
  template <typename U>
  explicit tvec3(const tvec2<U>& v);
  template <typename U>
  explicit tvec3(const tvec3<U>& v);
  template <typename U>
  tvec3(const math::tref3<U>& r);
  
  // This macro will generate all possible swizzle combinations of x and y.
  __eve_gen_swizzle3(x, y, z)
  
  T& operator[](eve::size index) { return (&x)[index]; }
  const T& operator[](eve::size index) const { return (&x)[index]; }

  template <typename U> tvec3<T>& operator+=(const U& v);
  template <typename U> tvec3<T>& operator+=(const tvec3<U>& v);
  template <typename U> tvec3<T>& operator-=(const U& v);
  template <typename U> tvec3<T>& operator-=(const tvec3<U>& v);
  template <typename U> tvec3<T>& operator*=(const U& v);
  template <typename U> tvec3<T>& operator*=(const tvec3<U>& v);
  template <typename U> tvec3<T>& operator/=(const U& v);
  template <typename U> tvec3<T>& operator/=(const tvec3<U>& v);
  template <typename U> tvec3<T>& operator=(const tvec3<U>& v);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef tvec3<real> vec3;
typedef tvec3<int8> vec3b;
typedef tvec3<uint8> vec3ub;
typedef tvec3<int16> vec3s;
typedef tvec3<uint16> vec3us;
typedef tvec3<int32> vec3i;
typedef tvec3<uint32> vec3u;
typedef tvec3<float> vec3f;
typedef tvec3<double> vec3d;

} // eve

/** @} */

#include "vec3.inl"
