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

#include "eve/string.h"
#include "eve/storage.h"
#include "eve/allocator.h"
#include <atomic>
#include <cstring>

#pragma warning (disable: 4996) // Unsafe strcpy

using namespace eve;

struct data
{
  std::atomic_size_t references;
  const char* rawdata;

  data(const char* rawdata) 
    : references(1), rawdata(rawdata)
  {
  }

  ~data()
  {
    eve::allocator::global().deallocate(rawdata);
  }
};

inline data* to_data(const void* ptr)
{
  return static_cast<data*>(const_cast<void*>(ptr));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

string::string()
  : m_data(""), m_size(0)
{
}

string::string(const char* data)
  : m_data(data), m_size(strlen(data))
{
}

string::string(const string& rhs)
  : m_data(rhs.m_data), m_size(rhs.m_size)
{
  if (is_reference())
    ++to_data(m_data)->references;
}

string::string(string&& rhs)
  : m_data(rhs.m_data), m_size(rhs.m_size)
{
  rhs.m_data = "";
  rhs.m_size = 0;
}

string::~string()
{
  clear();
}

const char* string::data() const
{
  if (is_reference())
    return to_data(m_data)->rawdata;
  else
    return static_cast<const char*>(m_data);
}

void string::clear()
{
  if (is_reference())
  {
    auto data = to_data(m_data);
    if (data->references.fetch_sub(1) == 1)
    {
      eve::destroy(data);
    }
  }
  m_data = "";
  m_size = 0;
}

bool string::operator==(const string& rhs) const
{
  return m_data == rhs.m_data || strcmp(data(), rhs.data()) == 0;
}

bool string::operator==(const char* rhs) const
{
  return m_data == rhs|| strcmp(data(), rhs) == 0;
}

string& string::operator=(const char* rhs)
{
  clear();
  m_data = rhs;
  m_size = strlen(rhs);
  return *this;
}

string& string::operator=(const string& rhs)
{
  clear();

  m_data = rhs.m_data;
  m_size = rhs.m_size;

  if (is_reference())
    ++to_data(m_data)->references;

  return *this;  
}

string& string::operator=(string&& rhs)
{
  clear();

  std::swap(m_data, rhs.m_data);
  std::swap(m_size, rhs.m_size);

  return *this;
}

string string::operator+(const char* rhs)
{
  return append(rhs, strlen(rhs));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

string::string(const char* rawdata, eve::size size)
  : m_data(eve_new ::data(rawdata)), m_size(size | eve::size_msb)
{
}

string string::append(const char* rhs, eve::size rhssize)
{
  auto totsize = size() + rhssize;
  eve::unique_alloc<eve::allocator::heap> alloc(eve::allocator::global(), totsize, 1);
  auto charbuffer = (char*)(void*)alloc;
  memcpy(charbuffer, data(), size());
  memcpy(charbuffer + size(), rhs, rhssize);
  charbuffer[totsize] = 0;
  string str((char*)((void*)alloc), totsize);
  alloc.release();
  return str;
}
