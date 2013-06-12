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

#ifdef _MSC_VER
#  define eve_aligned(_align) __declspec(align(_align))
#  define eve_alignof(...) __alignof(__VA_ARGS__)
#else
#  define eve_aligned(_align) __attribute__ ((aligned (_align)))
#  define eve_alignof(...) __alignof(__VA_ARGS__)
#endif

#ifdef EVE_32
#  define eve_sizeof(...) sizeof(__VA_ARGS__)
#else
#  define eve_sizeof(...) (eve::u32)(sizeof(__VA_ARGS__))
#endif

namespace eve
{

//// INT TYPES DEFINITIONS
typedef char i8;
typedef unsigned char u8;
typedef short i16;
typedef unsigned short u16;
typedef int i32;
typedef unsigned int u32;
typedef long long i64;
typedef unsigned long long u64;
typedef u32 size;

#ifdef EVE_32
typedef u32 uintptr;
#else
typedef u64 uintptr;
#endif

static const size size_msb = 1 << 31;

} // eve

/** }@ */
