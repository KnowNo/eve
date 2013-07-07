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
  #define eve_function_name __FUNCTION__
  #define eve_stdcall __stdcall
  #define eve_thread_local _declspec(thread)
#else
  #define eve_function_name __PRETTY_FUNCTION__
  #define eve_stdcall
  #define eve_thread_local __thread
#endif

  #define __eve_pp_stringify(arg) #arg
/** Turns arg into "arg". */
#define eve_pp_stringify(arg) __eve_pp_stringify(arg)

  #define _eve_pp_paste(a, b) a ## b
#define eve_pp_paste(a, b) _eve_pp_paste(a, b)
#define eve_pp_superpaste(a, b) eve_pp_paste(a, b)

#define eve_max2(a, b) ((a) > (b) ? (a) : (b))

#define eve_pp_echo(...) __VA_ARGS__

////////////////////////////////////////////////////////////////////////////////////////////////////

  #define _eve_pp_sequence \
           63,62,61,60,                   \
           59,58,57,56,55,54,53,52,51,50, \
           49,48,47,46,45,44,43,42,41,40, \
           39,38,37,36,35,34,33,32,31,30, \
           29,28,27,26,25,24,23,22,21,20, \
           19,18,17,16,15,14,13,12,11,10, \
           9,8,7,6,5,4,3,2,1,0
  #define _eve_pp_magic_mask( \
            _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
           _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
           _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
           _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
           _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
           _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
           _61,_62,_63,N,...) N
  #define _eve_pp_nargs(tuple) _eve_pp_magic_mask tuple

/** This macro gives the number of arguments of a variadic macro. */
#define eve_pp_nargs(...) _eve_pp_nargs((__VA_ARGS__, _eve_pp_sequence))

////////////////////////////////////////////////////////////////////////////////////////////////////

  #define _eve_pp_map_1(MACRO, arg) MACRO(arg)
  #define _eve_pp_map_2(MACRO, arg1, arg2) _eve_pp_map_1(MACRO, arg1) MACRO(arg2)
  #define _eve_pp_map_3(MACRO, arg1, arg2, arg3) _eve_pp_map_2(MACRO, arg1, arg2) MACRO(arg3)
  #define _eve_pp_map_4(MACRO, arg1, arg2, arg3, arg4) _eve_pp_map_3(MACRO, arg1, arg2, arg3) MACRO(arg4)
  #define _eve_pp_map_5(MACRO, arg1, arg2, arg3, arg4, arg5) _eve_pp_map_4(MACRO, arg1, arg2, arg3, arg4) MACRO(arg5)
  #define _eve_pp_map_6(MACRO, arg1, arg2, arg3, arg4, arg5, arg6) _eve_pp_map_5(MACRO, arg1, arg2, arg3, arg4, arg5) MACRO(arg6)
  #define _eve_pp_map_7(MACRO, arg1, arg2, arg3, arg4, arg5, arg6, arg7) _eve_pp_map_6(MACRO, arg1, arg2, arg3, arg4, arg5, arg6) MACRO(arg7)
  #define _eve_pp_map_8(MACRO, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) _eve_pp_map_7(MACRO, arg1, arg2, arg3, arg4, arg5, arg6, arg7) MACRO(arg8)
  #define _eve_pp_map_9(MACRO, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) _eve_pp_map_8(MACRO, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) MACRO(arg9)
  #define _eve_pp_map_10(MACRO, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10) _eve_pp_map_9(MACRO, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) MACRO(arg10)

#ifdef _MSC_VER
/** Joins and calls a MACRO with some (args). Works only on MVC. */
# define _eve_pp_call(MACRO, args) MACRO args
#endif

/** Applies each argument to MACRO. Es: XI_PP_MAP(F, a, b, c) expands to F(a) F(b) F(c) */
#ifdef _MSC_VER
# define eve_pp_map(MACRO, ...) _eve_pp_call(eve_pp_superpaste(_eve_pp_map_, eve_pp_nargs(__VA_ARGS__)), (MACRO, __VA_ARGS__))
#else
# define eve_pp_map(MACRO, ...) eve_pp_superpaste(_eve_pp_map_, eve_pp_nargs(__VA_ARGS__)) (MACRO, __VA_ARGS__)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

  #define _eve_pp_map_arg_1(MACRO, arg, arg1) MACRO(arg, arg1)
  #define _eve_pp_map_arg_2(MACRO, arg, arg1, arg2) _eve_pp_map_arg_1(MACRO, arg, arg1) MACRO(arg, arg2)
  #define _eve_pp_map_arg_3(MACRO, arg, arg1, arg2, arg3) _eve_pp_map_arg_2(MACRO, arg, arg1, arg2) MACRO(arg, arg3)
  #define _eve_pp_map_arg_4(MACRO, arg, arg1, arg2, arg3, arg4) _eve_pp_map_arg_3(MACRO, arg, arg1, arg2, arg3) MACRO(arg, arg4)
  #define _eve_pp_map_arg_5(MACRO, arg, arg1, arg2, arg3, arg4, arg5) _eve_pp_map_arg_4(MACRO, arg, arg1, arg2, arg3, arg4) MACRO(arg, arg5)
  #define _eve_pp_map_arg_6(MACRO, arg, arg1, arg2, arg3, arg4, arg5, arg6) _eve_pp_map_arg_5(MACRO, arg, arg1, arg2, arg3, arg4, arg5) MACRO(arg, arg6)
  #define _eve_pp_map_arg_7(MACRO, arg, arg1, arg2, arg3, arg4, arg5, arg6, arg7) _eve_pp_map_arg_6(MACRO, arg, arg1, arg2, arg3, arg4, arg5, arg6) MACRO(arg, arg7)
  #define _eve_pp_map_arg_8(MACRO, arg, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) _eve_pp_map_arg_7(MACRO, arg, arg1, arg2, arg3, arg4, arg5, arg6, arg7) MACRO(arg, arg8)
  #define _eve_pp_map_arg_9(MACRO, arg, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) _eve_pp_map_arg_8(MACRO, arg, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) MACRO(arg, arg9)
  #define _eve_pp_map_arg_10(MACRO, arg, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10) _eve_pp_map_arg_9(MACRO, arg, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) MACRO(arg, arg10)

/** Applies each argument to MACRO. Es: XI_PP_MAP(F, a, b, c) expands to F(a) F(b) F(c) */
#ifdef _MSC_VER
#  define eve_pp_map_arg(MACRO, arg, ...) _eve_pp_call(eve_pp_superpaste(_eve_pp_map_arg_, eve_pp_nargs(__VA_ARGS__)), (MACRO, arg, __VA_ARGS__))
#else
#  define eve_pp_map_arg(MACRO, arg, ...) eve_pp_superpaste(_eve_pp_map_arg_, eve_pp_nargs(__VA_ARGS__)) (MACRO, arg, __VA_ARGS__)
#endif

/** }@ */
