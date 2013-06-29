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

/** Only a helper function, actually outputs the two const and non-const functions. */
#define __eve_swizzle2_funcs(a, b)\
  tvec2<T> a##b() const { return tvec2<T>(a, b); }\
  math::tref2<T> a##b() { return math::tref2<T>(a, b); }

#define __eve_swizzle2_swap_funcs(a ,b)\
  __eve_swizzle2_funcs(a, b)\
  __eve_swizzle2_funcs(b, a)

/** Generates all possible swizzle functions of fields @p a and @p b. */
#define __eve_gen_swizzle2(a, b)\
  __eve_swizzle2_funcs(a, a)\
  __eve_swizzle2_swap_funcs(a, b)\
  __eve_swizzle2_funcs(b, b)

#define __eve_swizzle3_funcs(a, b, c)\
  tvec3<T> a##b##c() const { return tvec3<T>(a, b, c); }\
  math::tref3<T> a##b##c() { return math::tref3<T>(a, b, c); }

/** Generates all possible swizzle functions of fields @p a, @p b and @p c. */
#define __eve_gen_swizzle3(a, b, c)\
  __eve_swizzle2_funcs(a, a)\
  __eve_swizzle2_funcs(a, b)\
  __eve_swizzle2_funcs(a, c)\
  __eve_swizzle2_funcs(b, a)\
  __eve_swizzle2_funcs(b, b)\
  __eve_swizzle2_funcs(b, c)\
  __eve_swizzle2_funcs(c, a)\
  __eve_swizzle2_funcs(c, b)\
  __eve_swizzle2_funcs(c, c)\
  __eve_swizzle3_funcs(a, a, a)\
  __eve_swizzle3_funcs(a, a, b)\
  __eve_swizzle3_funcs(a, a, c)\
  __eve_swizzle3_funcs(a, b, a)\
  __eve_swizzle3_funcs(a, b, b)\
  __eve_swizzle3_funcs(a, b, c)\
  __eve_swizzle3_funcs(a, c, a)\
  __eve_swizzle3_funcs(a, c, b)\
  __eve_swizzle3_funcs(a, c, c)\
  __eve_swizzle3_funcs(b, a, a)\
  __eve_swizzle3_funcs(b, a, b)\
  __eve_swizzle3_funcs(b, a, c)\
  __eve_swizzle3_funcs(b, b, a)\
  __eve_swizzle3_funcs(b, b, b)\
  __eve_swizzle3_funcs(b, b, c)\
  __eve_swizzle3_funcs(b, c, a)\
  __eve_swizzle3_funcs(b, c, b)\
  __eve_swizzle3_funcs(b, c, c)\
  __eve_swizzle3_funcs(c, a, a)\
  __eve_swizzle3_funcs(c, a, b)\
  __eve_swizzle3_funcs(c, a, c)\
  __eve_swizzle3_funcs(c, b, a)\
  __eve_swizzle3_funcs(c, b, b)\
  __eve_swizzle3_funcs(c, b, c)\
  __eve_swizzle3_funcs(c, c, a)\
  __eve_swizzle3_funcs(c, c, b)\
  __eve_swizzle3_funcs(c, c, c)
