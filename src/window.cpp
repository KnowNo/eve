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

#include "eve/window.h"
#include "eve/utils.h"

#ifdef EVE_WINDOWS
#include "window/win32_window.inl"
#else
#error Implement eve::window on this platform.
#endif

using namespace eve;

eve_def_serializable(window::config, width, height, bpp, depth_bits, stencil_bits,
                     fsaa, fullscreen, resizable, vsync)

std::string window::config::path = "window.txt";
uint8 window::config::glmajor = 2;
uint8 window::config::glminor = 1;

window::config::config()
  : width(800)
  , height(600)
  , bpp(16)
  , depth_bits(16)
  , stencil_bits(0)
  , fsaa(0)
  , fullscreen(false)
  , resizable(true)
  , vsync(false)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

window::window(const std::string& title)
  : m_width(0)
  , m_height(0)
  , m_title(title)
{
  m_pimpl.construct<window_impl>();
}

window::~window()
{
  eve::destroy(&m_pimpl.as<window_impl>());
}

void window::configure(const config& config)
{
  m_config = config;
}

void window::configure(const std::string& filename)
{
  eve::deserialize_as_text(eve::open_fstream(filename), m_config);
}

bool window::opened() const
{
  return m_pimpl.as<window_impl>().opened();
}

void window::title(const std::string& title)
{
  m_title = title;
  if (opened())
  {
    m_pimpl.as<window_impl>().title(title);
  }
}

void window::open()
{
  m_pimpl.as<window_impl>().open(m_config, m_title.c_str());
  m_width = m_config.width;
  m_height = m_config.height;
}

bool window::poll(event& e)
{
  m_pimpl.as<window_impl>().poll(e);
  switch (e.type)
  {
    case event::SIZE:
      m_width = e.size.width;
      m_height = e.size.height;
      break;

    case event::KEYDOWN:
    case event::KEYUP:
      m_keyboard.handle_event(e.key.code, e.type == event::KEYDOWN);
      break;

      m_keyboard.handle_event(e.key.code, false);
      break;

    case event::MOUSEMOTION:
      m_mouse.handle_motion(e.mouse.cursor);
      break;

    case event::MOUSEDOWN:
    case event::MOUSEUP:
      m_mouse.handle_click(e.mouse.button, e.type == event::MOUSEDOWN);
      break;

    default: break;
  }
  return e.type != event::NONE;
}

void window::activate()
{
  m_pimpl.as<window_impl>().activate();
}

void window::display()
{
  m_pimpl.as<window_impl>().display();
}

void window::close()
{
  m_pimpl.as<window_impl>().close();
}
