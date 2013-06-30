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
#include "window/window_win32.h"
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

enum
{
  FLAG_OPEN = eve_bit(0),
  FLAG_CURSOR_HIDDEN = eve_bit(1),
  FLAG_FULLSCREEN = eve_bit(2),
};

window::window(const std::string& title)
  : m_width(0)
  , m_height(0)
  , m_title(title)
  , m_flags(0)
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
  return eve::flag(m_flags, FLAG_OPEN);
}

void window::title(const std::string& title)
{
  m_title = title;
  if (opened())
  {
    // TODO set title
  }
}

void window::open()
{
  m_pimpl.as<window_impl>().open(m_config, m_title.c_str());
  m_width = m_config.width;
  m_height = m_config.height;
  eve::flag(m_flags, FLAG_OPEN, true);
  eve::flag(m_flags, FLAG_FULLSCREEN, m_config.fullscreen);
}

bool window::poll(event& e)
{
  return m_pimpl.as<window_impl>().poll(e, eve::flag(m_flags, FLAG_FULLSCREEN));
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
  m_pimpl.as<window_impl>().close(m_config.fullscreen, eve::flag(m_flags, FLAG_CURSOR_HIDDEN));
  eve::flag(m_flags, FLAG_OPEN, false);
}
