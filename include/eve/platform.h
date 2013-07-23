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

//// Empty symbols used only for clarity in code ////
/** Argument is only read. */
#define eve_in
/** Argument is only written. */
#define eve_out
/** Argument is both read and written. */
#define eve_inout


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
#  define eve_sizeof(...) (eve::uint32)(sizeof(__VA_ARGS__))
#endif


//// Forced inlining ////
#if (defined(_MSC_VER))
#  define eve_inline __forceinline
#elif (defined(__GNUC__))
#  define eve_inline __attribute__((always_inline))
#else
#  define eve_inline inline
#endif

namespace eve {

// Real type definition (change this to suit your needs).
typedef float real;

//// INT TYPES DEFINITIONS
typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;
typedef uint32 size;
typedef uint32 id;

#ifdef EVE_32
typedef uint32 uintptr;
#else
typedef uint64 uintptr;
#endif

static const size size_msb = 1 << 31;

//// Arithmetic Types ////

/** A convenient enum of all primitive types. */
enum class arithmetic_type
{
  CHAR, UCHAR, SHORT, USHORT, INT, UINT, LONGLONG, ULONGLONG, FLOAT, DOUBLE
};

/** Returns the size of @p type arithmetic type. */
size arithmetic_type_size(arithmetic_type type);

//// Utils

/** Returns a unique unsigned integer value each time it is called. */
size unique_id();

} // eve

/** }@ */
