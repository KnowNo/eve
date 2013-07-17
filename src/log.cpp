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

#include "eve/log.h"
#include "eve/debug.h"
#include <iostream>
#include <fstream>
#include <mutex>

using namespace eve;

static std::string s_filename;
static std::ofstream s_file;
static std::ostream* s_output = &std::cout;
static std::mutex s_mutex;

void log::file(const std::string& filename)
{
  if (filename.empty())
  {
    s_file.close();
    s_output = &std::cout;
  } else
  {
    s_file.open(filename.c_str());
    s_output = &s_file;
  }
}

void log::info(const std::string& message)
{
  std::lock_guard<std::mutex> lock(s_mutex);
  *s_output << "[i] " << message << std::endl;
}

void log::warning(const std::string& message)
{
  std::lock_guard<std::mutex> lock(s_mutex);
  *s_output << "[!] " << message << std::endl;
}

void log::error(const std::string& message)
{
  std::lock_guard<std::mutex> lock(s_mutex);
  eve::show_error(message.c_str());
  *s_output << "[X] " << message << std::endl;
}

void log::fatal(const std::string& message)
{
  std::lock_guard<std::mutex> lock(s_mutex);
  *s_output << "[FATAL] " << message << std::endl;
  eve::abort(message.c_str());
}
