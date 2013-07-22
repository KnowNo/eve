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

#include "eve/text.h"
#include "eve/path.h"
#include <sstream>

using namespace eve;

void text::load(std::istream& input)
{
  m_text = "";
  std::string temp;

  unsigned lineoffset = 0;
  m_segments.clear();
  m_segments.emplace_back(0, 1);

  auto skipLine = [&input] ()
  {
    while (input.get() != '\n' && !input.eof())
    { }
  };

  while (true)
  {
    temp = "";
    char c = input.get();
    if (c == EOF)
      break;
    temp += c;
    
    if (c == '\n')
      ++m_segments.back().lineend;

    if (c == '#')
    {
      c = input.get();
      temp += c;
      if (c == 'e')
      {
        c = input.get();
        temp += c;
        if (c == 'v')
        {
          c = input.get();
          temp += c;
          if (c == 'e')
          {
            c = input.get();
            temp += c;
            if (c == '_')
            {
              std::string token = tokenize(input);
              if (token == "include")
              {
                std::string filename = read_string(input);
                
                if (filename.empty())
                  throw std::runtime_error("Filename is empty in include directive.");
                
                /** Calculate included text full path. */
                auto fullpath = this->path();
                eve::path::pop(fullpath);
                eve::path::push(fullpath, filename);

                /* Load included text */
                ptr included((resource_host*)this);
                included.load(fullpath);
                m_text += included->str();

                // TOD eve::rollback

                m_segments.emplace_back(included, m_segments.back().lineend - 1, m_segments.back().lineend);
                auto& seg = m_segments.back();

                /* Scan included text for newlines and increment the lineend of last segment. */
                for (char ic : included->str())
                  if (ic == '\n')
                    ++seg.lineend;
              
                /* eve_include final end of line */
                ++seg.lineend;

                /* adjust line offset */
                lineoffset += seg.lineend - seg.lineoffset - 2;
                
                /* push a new blank segment */
                m_segments.emplace_back(lineoffset, m_segments.back().lineend - 1);

              } else if (token == "define")
              {
                std::string key = tokenize(input);
                if (key.empty())
                  throw std::runtime_error("Invalid define directive.");
                std::string value = tokenize(input);
                m_defines[key] = value;
              } else
              {
                auto value = fetch_define(token);
                m_text += value;
              }
              continue;
            }
          }
        }
      }
    }
    m_text += temp;
  }
  ++m_segments.back().lineend;
}

void text::unload()
{
  m_text = "";
  m_defines.clear();
  m_segments.clear();
}

bool text::is_defined(const std::string& key) const
{
  auto it = m_defines.find(key);
  return it != m_defines.end();
}

const std::string& text::fetch_define(const std::string& key) const
{
  auto it = m_defines.find(key);
  if (it != m_defines.end())
    return it->second;
  static const std::string k_empty = "";
  return k_empty;
}

void text::on_reload()
{
  resource::source source(path());
  unload();
  load(*source);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

std::string text::translate(unsigned line, bool recursivecall) const
{
  for (const segment& seg: m_segments)
    if (line < seg.lineend)
      if (seg.included)
        return seg.included->translate(line - seg.lineoffset, true);
      else
      {
        std::stringstream ss;
        if (recursivecall)
          ss << this->path() << ":";
        ss << line - seg.lineoffset;
        return ss.str();
      }
  return "LINE_NOT_FOUND";
}

std::string text::tokenize(std::istream& input)
{
  std::string token;
  char c;
  skip_whitespaces(input);
  while (!input.eof())
  {
    c = input.get();
    if (c == -1 || c == ' ' || c == '\r' || c == '\t' || c == '\n')
      break;
    token += c;
  }
  return token;
}

std::string text::read_string(std::istream& input)
{
  skip_whitespaces(input);
  if (input.eof())
    throw std::runtime_error("parsing eve::text " + path() + ": no string found.");
  char c = input.get();
  if (c != '"')
    throw std::runtime_error("parsing eve::text " + path() + ": string does not begin with '\"'");
  c = input.get();
  std::string str;
  while (c != '\"' && !input.eof())
  {
    str += c;
    c = input.get();
  }
  if (c != '"')
    throw std::runtime_error("parsing eve::text " + path() + ": string does not end with '\"'");
  return str;
}

void text::skip_whitespaces(std::istream& input)
{
  while (!input.eof())
  {
    char c = input.get();
    if (c == -1)
      break;
    if (!(c == ' ' || c == '\r' || c == '\t' || c == '\n'))
      break;
  }
  input.unget();
}
