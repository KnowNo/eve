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
#include "../type_traits.h"

/** \addtogroup Math
  * @{
  */

namespace eve {

/** @returns the magnitude of @p x. */ 
template <typename T> 
typename inner_value_type<T>::type length(const T& x);

/** @returns the distance (a scalar) between @p a and @p b. */
template <typename T, typename U>
typename common_inner_type_to_float<T, U>::type distance(const T& a, const U& b)
{
  return static_cast<typename common_inner_type_to_float<T, U>::type>(length(a - b));
}

/** Normalizes @p v: after calling this its length() is equal to 1. */
template <typename T> 
void normalize(T& v);

/** @returns a normalized copy of @p v. */
template <typename T> 
T normalized(const T& v);

/** @returns the dot product between two vectors @p a and @p b with same dimension. */
template <typename T, typename U>
typename common_inner_type_to_float<T, U>::type dot(const T& a, const U& b);

/** Computes the cross product between @p a and @p b.
    @par
    - If a and b are two vec3s, the resulting cross product vec3 is returned.
    - If a and b are two vec2s, they're considered two vec3 with z = 0. The z of the
    resulting cross product is returned. */
template <typename T, typename U>
typename T cross(const T& a, const U& b);

} // eve

/** @} */

#include "funcgeo.inl"
