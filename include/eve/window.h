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

#include "serialization.h"
#include "storage.h"
#include "keyboard.h"
#include "mouse.h"
#include <functional>

/** \addtogroup Lib
  * @{
  */

namespace eve {

class window : uncopyable
{
public:
  struct config
  {
    static std::string path;
    static uint8 glmajor;
    static uint8 glminor;

    uint16 width;
    uint16 height;
    uint8 bpp;
    uint8 depth_bits;
    uint8 stencil_bits;
    uint8 fsaa;

    bool fullscreen;
    bool resizable;
    bool vsync;
    
    /** For default values initialization. */
    config();

    eve_decl_serializable
  };

  struct event;

public:
  /** @param title The starting window title. */
  window(const std::string& title = "eve");
  ~window();

  /** @returns this window current width. */
  uint16 width() const { return m_width; }

  /** @returns this window current height. */
  uint16 height() const { return m_height; }

  /** @returns the keyboard instance associated to this window. */
  const eve::keyboard& keyboard() const { return m_keyboard; }

  /** @returns the mouse instance associated to this window. */
  const eve::mouse& mouse() const { return m_mouse; }

  /** Configures this window using the specified @p config. */
  void configure(const config& config);
  
  /** Configures this window using the window::config at @p filename. */
  void configure(const std::string& filename);

  /** @returns whether the window is open (open() was called successfully and
      close() has not been called yet). */
  bool opened() const;

  /** Sets the window title to @p title. Title can be set even if the window is 
      not open yet. */
  void title(const std::string& title);

  /** Opens the window using configuration file at eve::config::path.  */
  void open();

  /** Pops a window event into @p e from the queue. 
      @returns whether or not an event has occurred. */
  bool poll(event& e);

  /** Makes this window the current rendering context. All following rendering
      commands will target this window. */
  void activate();

  /** Swaps the back with the front buffer and showes rendered frame on the
      screen. */
  void display();

  /** Closes the window if open otherwise does nothing. */
  void close();

private:
  config m_config;
  std::string m_title;
  uint16 m_width;
  uint16 m_height;
  eve::fixed_storage<64> m_pimpl;
  eve::keyboard m_keyboard;
  eve::mouse m_mouse;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct window::event
{
  enum
  {
    /** Internal usage, polled event will never have this type. */
    NONE,

    /** Occurs when the close button is pressed. */
    QUIT,

    /** Occurs when the window resizing ends. Fields 'width and 'height'
        have the new dimensions.*/
    SIZE,

    /** Occurs when a key is pressed. */
    KEYDOWN,

    /** Occurs when a key is released. */
    KEYUP,

    /** Occurs when the mouse cursor is moved. */
    MOUSEMOTION,

    /** Occurs when a mouse button is pressed. */
    MOUSEDOWN,

    /** Occurs when a mouse button is released. */
    MOUSEUP,

  } type;
    
  struct size_event
  {
    uint16 width;
    uint16 height;
  };

  struct key_event
  {
    uint16 repetitions;
    key code;
    uint16 scancode;
    uint8 mods;
  };

  struct mouse_event
  {
    eve::vec2i cursor;
    mouse::button button;
  };

  union
  {
    size_event size;
    key_event key;
  };
  mouse_event mouse;

  event() : type(NONE) { }
};

} // eve

/** }@ */
