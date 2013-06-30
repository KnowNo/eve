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

#include "math/detail/vec2.h"

namespace eve
{

class mouse
{
public:
  enum class button
  {
    LEFT, MIDDLE, RIGHT, BUTTON_4, BUTTON_5, _LAST
  };

  /** @retursn the current mouse cursor position. */
  const vec2i& position() const { return m_position; }

  /** @returns the current mouse button @btn state. True for pressed, false for released. */
  bool buttonstate(button btn) const { return m_buttons[eve::size(btn)]; }

private:
  mouse();

  /** Called by eve::window. Sets the current mouse position. */
  void handle_motion(const eve::vec2i& position);

  /** Called by eve::window. Sets the mouse button state (true pressed, false relesed). */
  void handle_click(button btn, bool state);

  eve::vec2i m_position;
  bool m_buttons[eve::size(button::_LAST)];

  friend class window;
};

} // eve
