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

#include "application.h"
#include "window.h"
#include <unordered_map>

/** \addtogroup Lib
  * @{
  */

namespace eve {

class game
{
public:
  struct time;
  class state;

  game(eve::flagset<application::module> modules = application::module::graphics | application::module::memory_debugger);
  ~game();

  eve::window& window() { return m_window; }
  const eve::window& window() const { return m_window; }

  template <typename State, typename... Args>
  void create_state(Args&&... args)
  {
    const auto stateID = eve::type_id<State>();
    auto it = m_states.find(stateID);
    if (it != m_states.end())
      throw std::runtime_error("State with ID " + std::to_string(stateID) + " already created.");
    it = m_states.insert(it, std::make_pair(eve::type_id<State>(), eve_new State(this)));
  }

  template <typename State, typename... Args>
  void run(Args&&... args)
  {
    eve_assert(!m_top);
    initialize();
    transit<State>(std::forward<Args>(args)...);
    mainloop();
  };

  template <typename State, typename... Args>
  void transit(Args&&... args)
  {
    const auto stateID = eve::type_id<State>();
    auto it = m_states.find(stateID);
    if (it == m_states.end())
      throw std::runtime_error("State with id " + std::to_string(stateID) + " not created.");
    static_cast<State*>(it->second)->setup(std::forward<Args>(args)...);
    transit(it->second);
  }

private:
  void initialize();
  void mainloop();
  void transit(state*);

  eve::application m_app;
  eve::window m_window;
  std::unordered_map<eve::id, state*> m_states;
  state* m_top;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct game::time
{
  float elapsed;
  unsigned fps; 
  bool fps_changed;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class game::state
{
public:
  virtual ~state() {}

  virtual void update(const game::time& time) = 0;
  virtual void draw(const game::time& time) const = 0;

  virtual void setup() {}
  virtual void on_activated() {}
  virtual void on_deactivated() {}
  
protected:
  state(game* game);
  game* game() const { return m_game; }

private:
  eve::game* m_game;
  state* m_next;
  friend eve::game;
};

} // eve

/** }@ */
