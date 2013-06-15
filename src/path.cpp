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

#include "eve/path.h"

void eve::path::push(std::string& lhs, const std::string& rhs)
{
  if (lhs.empty())
    return;

  auto ch = lhs[lhs.size() - 1];
  if (ch == '/' || ch == '\\')
    lhs.pop_back();

  while (lhs.size() >= 2 && lhs[lhs.size() - 1] == '.' && (lhs[lhs.size() - 2] == '/' || lhs[lhs.size() - 2] == '\\' ))
    lhs.resize(lhs.size() - 2);

  if (lhs.back() != '\\' && lhs.back() != '/')
    lhs += '/';
  if (rhs[0] == '.' && (rhs[1] == '/' || rhs[1] == '\\'))
    lhs.append(rhs.c_str() + 2, rhs.size() - 2);
  else if (rhs.front() == '\\' || rhs.front() == '/')
    lhs.append(rhs.c_str() + 1, rhs.size() - 1);
  else
    lhs += rhs;
}

std::string eve::path::pop(std::string& path)
{
  size_t i = path.size() - 1;
  for (; i != size_t(-1); --i)
    if (path[i] == '\\' || path[i] == '/')
      break;
  if (i > path.size())
    return "";
  std::string filename = path.substr(i + 1, path.size() - i - 1);
  path = path.substr(0, i);
  return std::move(filename);
}
