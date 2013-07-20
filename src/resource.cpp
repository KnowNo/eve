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

#include "eve/resource.h"
#include "eve/debug.h"
#include "eve/exceptions.h"
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <iostream>
#include <fstream>

using namespace eve;

//// STATIC RESOURCES DATA
eve::size s_version = 0;
std::unordered_map<std::string, eve::resource*> s_resources;

////////////////////////////////////////////////////////////////////////////////////////////////////

static const std::string kEmptyString;

const std::string& eve::resource_host::path() const
{
  return kEmptyString;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

resource::source::source()
  : m_source(nullptr)
{
}

resource::source::source(const std::string& path)
  : m_source(nullptr)
{
  open(path);
}

void resource::source::open(const std::string& path)
{
  m_file.open(path.c_str(), std::ios::binary);
  if (!m_file.good())
    throw eve::file_not_found_error(path);
  m_source = &m_file;
}

////////////////////////////////////////////////////////////////////////////////

resource::resource()
  : m_references(0)
{
}

resource::~resource()
{
  eve_assert(m_references == 0);
  unload();
}

range<resource_host**> resource::dependants()
{
  if (m_dependants.empty())
    return range<resource_host**>();
  else
    return range<resource_host**>(&m_dependants[0], &m_dependants[0] + m_dependants.size());
}

void resource::reload()
{
  std::vector<resource_host*> queue;
  std::unordered_set<resource_host*> marks;
  std::function<void(resource_host*)> visit;
  visit = [&queue, &marks, &visit](resource_host* host)
  {
    auto it = marks.find(host);
    
    if (it != marks.end())
      return;
    
    marks.insert(it, host);

    for (auto dependant : host->dependants())
      visit(dependant);

    queue.push_back(host);
  };

  visit(this);

  for (auto it = queue.rbegin(); it != queue.rend(); ++it)
    (*it)->on_reload();
}

void resource::unload()
{
  m_valid = false;
}

resource* resource::find(const std::string& path)
{
  auto it = s_resources.find(path);
  if (it == s_resources.end())
    return nullptr;
  return it->second;
}

resource* resource::process_new(eve::unique_ptr<resource>::type res)
{
  res->m_valid = false;
  try
  {
    resource::source source(res->m_path);
    if (!(*source).good())
      throw std::runtime_error("file \"" + res->m_path + "\" not good for reading.");
    res->load(*source);
    res->m_valid = true;
  } catch (std::exception& e)
  {
    // TODO add logging
    std::cout << "resource loading error: " << e.what() << '\n';
  }
  s_resources[res->m_path] = res.get();
  return res.release();
}

void resource::add_dependant(resource_host* dependant)
{
  if (std::find(m_dependants.begin(), m_dependants.end(), dependant) != m_dependants.end())
    return;

  m_dependants.push_back(dependant);
}

void resource::remove_dependant(resource_host* dependant)
{
  auto it = std::find(m_dependants.begin(), m_dependants.end(), dependant);
  if (it == m_dependants.end())
    return;
  size_t index = it - m_dependants.begin();
  std::swap(m_dependants[index], m_dependants.back());
  m_dependants.pop_back();
}
