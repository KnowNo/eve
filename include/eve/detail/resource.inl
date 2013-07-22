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

#include "../path.h"

namespace eve {

template <class T, class Param> template<class... Args>
resource::ptr<T, Param>::ptr(Args&&... args)
  : m_host(nullptr)
  , m_resource(nullptr)
  , m_helper(std::forward<Args>(args)...)
{
}

template <class T, class Param> template<class... Args>
resource::ptr<T, Param>::ptr(resource_host* host, Args&&... args)
  : m_host(host)
  , m_resource(nullptr)
  , m_helper(std::forward<Args>(args)...)
{
}

template <class T, class Param>
resource::ptr<T, Param>::ptr(const ptr& rhs)
  : m_resource(nullptr)
{
  *this = rhs;
}

template <class T, class Param>
resource::ptr<T, Param>::ptr(ptr&& rhs)
  : m_resource(nullptr)
{
  *this = std::forward<ptr>(rhs);
}

template <class T, class Param>
resource::ptr<T, Param>::~ptr()
{
  reset(nullptr);
}

template <class T, class Param>
void resource::ptr<T, Param>::load(const std::string& path)
{
  auto res = static_cast<T*>(resource::find(path));
  if (!res)
  {
    eve::unique_ptr<resource>::type newres;
    newres.reset(m_helper.create_resource());
    newres->m_path = path;
    res = static_cast<T*>(resource::process_new(std::move(newres)));
  }
  reset(res);
  ++m_resource->m_references;
}

template <class T, class Param>
resource::ptr<T, Param>& resource::ptr<T, Param>::operator=(const ptr& rhs)
{
  m_host = rhs.m_host;
  reset(rhs.m_resource);
  if (m_resource)
    ++m_resource->m_references;
  return *this;
}

template <class T, class Param>
resource::ptr<T, Param>& resource::ptr<T, Param>::operator=(ptr&& rhs)
{
  m_host = rhs.m_host;
  reset(rhs.m_resource);
  rhs.m_resource = nullptr;
  return *this;
}

template <class T, class Param>
void resource::ptr<T, Param>::reset(T* res)
{
  if (m_resource)
  {
    if (m_host)
      m_resource->remove_dependant(m_host);

    if (--m_resource->m_references == 0)
    {
      m_resource->unload();
      eve::destroy(m_resource);
    }
  }

  m_resource = res;  

  if (m_resource && m_host)
    m_resource->add_dependant(m_host);
}


template <class T, class Param>
T* resource::helper<T, Param>::create_resource()
{
  return eve_new T(param);
}

template<class T>
struct resource::helper<T, void>
{
  T* create_resource()
  {
    return eve_new T;
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Param>
class text_serializer<resource::ptr<T, Param>>
{
public:
  static void serialize(const resource::ptr<T, Param>&, std::ostream&, const std::string&)
  {
    throw std::logic_error("Cannot serialize a resource::ptr. Implement this maybe?");
  }

  static void deserialize(serialization::parser& parser, resource::ptr<T, Param>& instance)
  {
    parser.check(parser.STRING);
    std::string path = instance.host()->path();
    eve::path::pop(path);
    eve::path::push(path, parser.token());
    parser.scan();
    instance.load(path);
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
void deserializable_resource<T>::load(std::istream& source)
{
  eve::deserialize_as_text<T>(source, static_cast<T&>(*this));
}

template<class T>
void deserializable_resource<T>::on_reload()
{
  resource::source source(path());
  load(*source);
}

} // eve
