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

#include "eve/game.h"
#include "eve/resource.h"
#include "eve/time.h"

using namespace eve;

game::game(const std::string& name, eve::flagset<application::module> modules)
  : m_app(modules)
  , m_top(nullptr)
{
  m_window.title(name);
}

game::~game()
{
  for (auto& state : m_states)
    eve::global_destroy(state.second);
  m_states.clear();
}

void game::initialize()
{
  m_window.open();
}

void game::mainloop()
{
  stopwatch stopwatch;
  fpscounter fps;
  window::event e;
  bool running = true;
  
  game::time time;
  time.elapsed = 0;
  time.fps_changed = false;
  time.fps = 0;
  
  while (true)
  {
    while (m_window.poll(e))
    {
      if (e.type == window::event::QUIT)
        running = false;
    }

    if (!running)
      break;

    // Make our window active
    m_window.activate();

    // Update logic
    m_top->update(time);

    // Render scene
    m_top->draw(time);

    // Swap buffers and display rendered frame
    m_window.display();

    // Update time measuring
    eve::time::fps_wait((float)stopwatch.elapsed(), 60);
    stopwatch.reset();
    time.fps_changed = fps.tick();
    time.fps = fps.value();
  };
}

void game::transit(state* state)
{
  if (m_top)
    m_top->on_deactivated();

  state->m_next = m_top;
  m_top = state;

  m_top->on_activated();
}

