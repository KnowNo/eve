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

namespace eve {

namespace math {

template <typename T>
tref2<T>::tref2(T& x, T& y)
  : x(x), y(y)
{
}

template <typename T>
tref2<T>::tref2(const tref2<T>& r)
  : x(r.x), y(r.y)
{
}

template <typename T>
tref2<T>& tref2<T>::operator=(const tref2<T>& r)
{
  x = r.x;
  y = r.y;
  return *this;
}

template <typename T>
template <typename U>
tref2<T>& tref2<T>::operator=(const tvec2<U>& v)
{
  x = T(v.x);
  y = T(v.y);
  return *this;
}

} // math

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
eve_inline tvec2<T>::tvec2()
  : x(T(0))
  , y(T(0))
{}

template <typename T>
template <typename U>
eve_inline tvec2<T>::tvec2(const U& x)
  : x(T(x))
  , y(T(x))
{}

template <typename T>
template <typename U, typename V>
eve_inline tvec2<T>::tvec2(const U& x, const V& y)
  : x(T(x))
  , y(T(y))
{}

template <typename T>
template <typename U>
eve_inline tvec2<T>::tvec2(const tvec2<U>& v)
  : x(T(v.x))
  , y(T(v.y))
{}

template <typename T>
template <typename U>
eve_inline tvec2<T>& tvec2<T>::operator+=(const U& sv)
{
  auto ts = T(s);
  x += ts;
  y += ts;
  return *this;
}

template <typename T>
template <typename U>
eve_inline tvec2<T>& tvec2<T>::operator+=(const tvec2<U>& v)
{
  x += T(v.x);
  y += T(v.x);
  return *this;
}

template <typename T>
template <typename U>
eve_inline tvec2<T>& tvec2<T>::operator-=(const U& s)
{
  auto ts = T(s);
  x -= ts;
  y -= ts;
  return *this;
}

template <typename T>
template <typename U>
eve_inline tvec2<T>& tvec2<T>::operator-=(const tvec2<U>& v)
{
  x -= T(v.x);
  y -= T(v.x);
  return *this;
}

template <typename T>
template <typename U>
eve_inline tvec2<T>& tvec2<T>::operator*=(const U& s)
{
  auto ts = T(s);
  x *= ts;
  y *= ts;
  return *this;
}

template <typename T>
template <typename U>
eve_inline tvec2<T>& tvec2<T>::operator*=(const tvec2<U>& v)
{
  x *= T(v.x);
  y *= T(v.x);
  return *this;
}

template <typename T>
template <typename U>
eve_inline tvec2<T>& tvec2<T>::operator/=(const U& s)
{
  auto ts = T(s);
  x /= ts;
  y /= ts;
  return *this;
}

template <typename T>
template <typename U>
eve_inline tvec2<T>& tvec2<T>::operator/=(const tvec2<U>& v)
{
  x /= T(v.x);
  y /= T(v.x);
  return *this;
}

template <typename T>
template <typename U>
eve_inline tvec2<T>& tvec2<T>::operator=(const tvec2<U>& v)
{
  this->x = T(v.x);
  this->y = T(v.y);
  return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

//// operator -
template<typename T, typename U>
eve_inline tvec2<T> operator+(const tvec2<T>& v, const U& s)
{
  return tvec2<T>(
    v.x + T(s),
    v.y + T(s));
}

template<typename T, typename U>
eve_inline tvec2<T> operator+(const U& s, const tvec2<T>& v)
{
  return tvec2<T>(
    T(s) + v.x,
    T(s) + v.y);
}

template<typename T, typename U>
eve_inline tvec2<T> operator+(const tvec2<T>& v1, const tvec2<U>& v2)
{
  return tvec2<T>(
    v1.x + T(v2.x),
    v1.y + T(v2.y));
}

//// operator+
template<typename T, typename U>
eve_inline tvec2<T> operator-(const tvec2<T>& v, const U& s)
{
  return tvec2<T>(
    v.x - s,
    v.y - s);
}

template<typename T, typename U>
eve_inline tvec2<T> operator-(const U& s, const tvec2<T>& v)
{
  return tvec2<T>(
    T(s) - v.x,
    T(s) - v.y);
}

template<typename T, typename U>
eve_inline tvec2<T> operator-(const tvec2<T>& v1, const tvec2<U>& v2)
{
  return tvec2<T>(
    v1.x - T(v2.x),
    v1.y - T(v2.y));
}

//// operator*
template<typename T, typename U>
eve_inline tvec2<T> operator*(const tvec2<T>& v, const U& s)
{
  return tvec2<T>(
    v.x * T(s),
    v.y * T(s));
}

template<typename T, typename U>
eve_inline tvec2<T> operator*(const U& s, const tvec2<T>& v)
{
  return tvec2<T>(
    T(s) * v.x,
    T(s) * v.y);
}

template<typename T, typename U>
eve_inline tvec2<T> operator*(const tvec2<T>& v1, const tvec2<U>& v2)
{
  return tvec2<T>(
    v1.x * T(v2.x),
    v1.y * T(v2.y));
}

//// operator/
template<typename T, typename U>
eve_inline tvec2<T> operator/(const tvec2<T>& v, const U& s)
{
  return tvec2<T>(
    v.x / T(s),
    v.y / T(s));
}

template<typename T, typename U>
eve_inline tvec2<T> operator/(const U& s, const tvec2<T>& v)
{
  return tvec2<T>(
    T(s) / v.x,
    T(s) / v.y);
}

template<typename T, typename U>
eve_inline tvec2<T> operator/(const tvec2<T>& v1, const tvec2<U>& v2)
{
  return tvec2<T>(
    v1.x / T(v2.x),
    v1.y / T(v2.y));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct inner_value_type<eve::math::tref2<T>>
{
  typedef T type;
};

template<typename T>
struct inner_value_type<tvec2<T>>
{
  typedef T type;
};

} // eve
