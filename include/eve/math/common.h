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

#include "../type_traits.h"

/** Only a helper function, actually outputs the two const and non-const functions. */
#define __eve_swizzle2_funcs(a, b)\
  tvec2 a##b() const { return tvec2(a, b); }\
  math::tref2<T> a##b() { return math::tref2<T>(a, b); }

/** Generates all possible swizzle functions of fields @p a and @p b. */
#define __eve_gen_swizzle2(a, b)\
  __eve_swizzle2_funcs(a, a)\
  __eve_swizzle2_funcs(a, b)\
  __eve_swizzle2_funcs(b, a)\
  __eve_swizzle2_funcs(b, b)\
  

/** \addtogroup Math
  * @{
  */

namespace eve {

template <typename T>
struct inner_value_type
{
  typedef T type;
};

template <typename T>
struct inner_value_type_to_float
{
  typedef typename inner_value_type<T>::type type;
};

template <typename... Types>
struct common_inner_type_to_float;

template <>
struct common_inner_type_to_float<>
{
  typedef float type;
};

template <typename Head, typename... Tail>
struct common_inner_type_to_float<Head, Tail...>
{
  typedef typename eve::tofloat<
    typename std::common_type<
      typename inner_value_type<Head>::type,
      typename common_inner_type_to_float<Tail...>::type
    >::type
  >::type type;
};

} // eve

/** @} */
