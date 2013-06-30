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

#include "resource.h"
#include "storage.h"
#include "detail/keyboard.h"
#include <functional>

/** \addtogroup Lib
  * @{
  */

namespace eve {

class window : protected resource_host
{
public:
  struct config : public deserializable_resource<config>
  {
    static std::string path;
    static u8 glmajor;
    static u8 glminor;

    u16 width;
    u16 height;
    u8 bpp;
    u8 depth_bits;
    u8 stencil_bits;
    u8 fsaa;

    bool fullscreen;
    bool resizable;
    bool vsync;
    
    /** For default values initialization. */
    config();

    eve_decl_serializable
  };

  struct event
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
      MOUSEMOVE,

      /** Occurs when a mouse button is either pressed or released. */
      MOUSEBUTTON,

    } type;
    
    struct size_event
    {
      u16 width;
      u16 height;
    };

    struct key_event
    {
      u16 repetitions;
      key code;
      u16 scancode;
      u8 mods;
    };

    union
    {
      size_event size;
      key_event key;
    };

    event() : type(NONE) { }
  };

public:
  /** @param title The starting window title. */
  window(const std::string& title = "eve");
  ~window();

  u16 width() const { return m_width; }
  u16 height() const { return m_height; }

  /** @returns whether the window is open (open() was called successfully and
      close() has not been called yet). */
  bool opened() const;

  /** Sets the window title to @p title. Title can be set even if the window is 
      not open yet. */
  void title(const std::string& title);

  /** Opens the window using configuration file at eve::config::path.  */
  void open();

  /** Pops a window event into @e from the queue. 
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

  /** Called when the configuration file is changed. */
  void on_reload() override;

  eve::resource::ptr<config> m_config;
  std::string m_title;
  u16 m_width;
  u16 m_height;
  u8 m_flags; // open? cursor captured?
  eve::fixed_storage<64> m_pimpl;
};

} // eve

/** }@ */