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

namespace eve {

enum class key : eve::uint16
{
  UNKNOWN = 0xFFFF,
  SPACE = 32,
  APOSTROPHE = 39,
  COMMA = 44,
  MINUS = 45,
  PERIOD = 46,
  SLASH = 47,

  /* Printable keys */
  KEY_0 = 48,
  KEY_1 = 49,
  KEY_2 = 50,
  KEY_3 = 51,
  KEY_4 = 52,
  KEY_5 = 53,
  KEY_6 = 54,
  KEY_7 = 55,
  KEY_8 = 56,
  KEY_9 = 57,
  SEMICOLON = 59,
  EQUAL = 61,
  KEY_A = 65,
  KEY_B = 66,
  KEY_C = 67,
  KEY_D = 68,
  KEY_E = 69,
  KEY_F = 70,
  KEY_G = 71,
  KEY_H = 72,
  KEY_I = 73,
  KEY_J = 74,
  KEY_K = 75,
  KEY_L = 76,
  KEY_M = 77,
  KEY_N = 78,
  KEY_O = 79,
  KEY_P = 80,
  KEY_Q = 81,
  KEY_R = 82,
  KEY_S = 83,
  KEY_T = 84,
  KEY_U = 85,
  KEY_V = 86,
  KEY_W = 87,
  KEY_X = 88,
  KEY_Y = 89,
  KEY_Z = 90,
  LEFT_BRACKET = 91,
  BACKSLASH = 92,
  RIGHT_BRACKET = 93,
  GRAVE_ACCENT = 96,
  WORLD_1 = 161,
  WORLD_2 = 162,

  /* Function keys */
  ESCAPE = 256,
  ENTER = 257,
  TAB = 258,
  BACKSPACE = 259,
  INSERT = 260,
  DELETE = 261,
  RIGHT = 262,
  LEFT = 263,
  DOWN = 264,
  UP = 265,
  PAGE_UP = 266,
  PAGE_DOWN = 267,
  HOME = 268,
  END = 269,
  CAPS_LOCK = 280,
  SCROLL_LOCK = 281,
  NUM_LOCK = 282,
  PRINT_SCREEN = 283,
  PAUSE = 284,
  F1 = 290,
  F2 = 291,
  F3 = 292,
  F4 = 293,
  F5 = 294,
  F6 = 295,
  F7 = 296,
  F8 = 297,
  F9 = 298,
  F10 = 299,
  F11 = 300,
  F12 = 301,
  F13 = 302,
  F14 = 303,
  F15 = 304,
  F16 = 305,
  F17 = 306,
  F18 = 307,
  F19 = 308,
  F20 = 309,
  F21 = 310,
  F22 = 311,
  F23 = 312,
  F24 = 313,
  F25 = 314,
  NUMPAD_0 = 320,
  NUMPAD_1 = 321,
  NUMPAD_2 = 322,
  NUMPAD_3 = 323,
  NUMPAD_4 = 324,
  NUMPAD_5 = 325,
  NUMPAD_6 = 326,
  NUMPAD_7 = 327,
  NUMPAD_8 = 328,
  NUMPAD_9 = 329,
  NUMPAD_DECIMAL = 330,
  NUMPAD_DIVIDE = 331,
  NUMPAD_MULTIPLY = 332,
  NUMPAD_SUBTRACT = 333,
  NUMPAD_ADD = 334,
  NUMPAD_ENTER = 335,
  NUMPAD_EQUAL = 336,
  LEFT_SHIFT = 340,
  LEFT_CTRL = 341,
  LEFT_ALT = 342,
  LEFT_SUPER = 343,
  RIGHT_SHIFT = 344,
  RIGHT_CTRL = 345,
  RIGHT_ALT = 346,
  RIGHT_SUPER = 347,
  MENU = 348,
  INVALID,
};

} // eve
