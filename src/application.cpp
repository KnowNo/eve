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

#include "eve/application.h"
#include "eve/resource.h"

using namespace eve;

namespace eve {

extern void initialize_platform();

extern void initialize_memory_debugger(bool enabled);
extern void terminate_memory_debugger();

extern void initialize_window();
extern void terminate_window();

extern void initialize_net();
extern void terminate_net();

extern void terminate_resources();

} // eve

application::application(eve::flagset<application::module> modules)
  : m_modules(modules)
{
  initialize_platform();
  initialize_memory_debugger(modules.isset(module::memory_debugger));
  if (modules.isset(module::graphics))
    initialize_window();
  if (modules.isset(module::networking))
    initialize_net();
}

application::~application()
{
  eve::terminate_resources();
  if (m_modules.isset(module::networking))
    terminate_net();
  if (m_modules.isset(module::graphics))
    terminate_window();
  eve::terminate_memory_debugger();
}
