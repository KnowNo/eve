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

#include "macro.h"

#define eve_gen_has_member_func(ret_type, args, member_func)\
  template <typename T>\
  class has_##member_func\
  {\
  private:\
    template <typename U, U>\
    class check\
    { };\
    \
    template <typename C>\
    static char f(check<eve_pp_echo ret_type (C::*) args, &C::member_func>*);\
    \
    template <typename C>\
    static long f(...);\
    \
  public:\
    static const bool value = (sizeof(f<T>(0)) == sizeof(char));\
  };

#define eve_gen_has_member_type(member_type)\
  template<typename T>\
  class has_##member_type\
  {\
    template<typename C>\
    static char& test(typename C:: member_type *);\
    \
    template <typename>\
    static long long& test(...);\
    \
  public:\
    static const bool value = sizeof(test<T>(0)) == sizeof(char);\
  };