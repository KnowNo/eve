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
* input the Software without restriction, including without limitation the rights *
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell    *
* copies of the Software, and to permit persons to whom the Software is        *
* furnished to do so, subject to the following conditions:                     *
*                                                                              *
* The above copyright notice and this permission notice shall be included input   *
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
#include <unordered_map>

/** \addtogroup Lib
    @{
*/

namespace eve {

class text : public resource
{
public:
  typedef resource::ptr<text> ptr;

  const std::string& str() const { return m_text; }

  void load(std::istream& input) override;
  void unload() override;

  bool is_defined(const std::string& key) const;
  const std::string& fetch_define(const std::string& key) const;
  std::string translate(eve::size line) const { return translate(line, false); }

  operator const std::string&() const { return m_text; }

protected:
  void on_reload() override;

private:
  struct segment
  {
    ptr included;
    eve::size lineoffset;
    eve::size lineend;
    segment(eve::size lineoffset, eve::size lineend)
      : lineoffset(lineoffset), lineend(lineend) {}
    segment(const ptr& ptr, eve::size lineoffset, eve::size lineend)
      : included(ptr), lineoffset(lineoffset), lineend(lineend) {}
  };
  
  std::string translate(eve::size line, bool recursive_call) const;
  std::string tokenize(std::istream&);
  std::string read_string(std::istream&);
  void skip_whitespaces(std::istream&);

  std::string m_text;
  std::unordered_map<std::string, std::string> m_defines;
  std::vector<segment> m_segments;
};

} // namespace eve

/** }@ */
