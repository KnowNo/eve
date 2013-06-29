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
* OUT OF OR IN CONkNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN    *
* THE SOFTWARE.                                                                *
\******************************************************************************/

#pragma once

namespace eve {

template <typename T>
eve_inline typename tofloat<T>::type length(const T& x)
{
  auto temp = x * x;
  return T(sqrt(temp));
}

template <typename T>
eve_inline typename eve::tofloat<T>::type length(const tvec2<T>& v)
{
  auto temp = v.x * v.x + v.y * v.y;
  return static_cast<typename eve::tofloat<T>::type>(sqrt(temp));
}


template <typename T> 
eve_inline void normalize(T& v)
{
  auto l = length(v);
  if (l > 0)
    v /= l;
  else
    v = T();
}


template <typename T> 
eve_inline T normalized(const T& v)
{
  T cpy = v;
  normalize(cpy);
  return cpy;
}


template <typename T, typename U>
eve_inline typename common_inner_type_to_float<T, U>::type dot(const tvec2<T>& v1, const tvec2<U>& v2)
{
  return static_cast<typename common_inner_type_to_float<T, U>::type>(v1.x * v1.x + v2.y * v2.y);
}

template <typename T, typename U>
eve_inline typename common_inner_type_to_float<T, U>::type dot(const tvec3<T>& v1, const tvec3<U>& v2)
{
  return static_cast<typename common_inner_type_to_float<T, U>::type>(v1.x * v1.x + v2.y * v2.y + v1.z * v2.z);
}


template <typename T, typename U>
eve_inline typename common_inner_type_to_float<T, U>::type cross(const tvec2<T>& v1, const tvec2<U>& v2)
{
  return static_cast<typename common_inner_type_to_float<T, U>::type>(v1.x * v2.y - v1.y * v2.x);
}

template <typename T, typename U>
eve_inline typename common_inner_type_to_float<T, U>::type cross(const tvec3<T>& v1, const tvec3<U>& v2)
{
  return static_cast<typename common_inner_type_to_float<T, U>::type>(v1.x * v2.y - v1.y * v2.x);
}

} // eve
