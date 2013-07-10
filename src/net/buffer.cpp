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

#include "eve/net/buffer.h"
#include "eve/net/socket.h"
#include "eve/allocator.h"

using namespace eve::net;

buffer::buffer(eve::socket* socket, eve::size capacity)
  : m_socket(socket)
{
  capacity = std::max<eve::size>(capacity, 2);
  auto doublecapacity = capacity * 2;

  m_buffer = (char*)eve::allocator::global().allocate(eve_here, doublecapacity, 1);
  setg(m_buffer, m_buffer + capacity, m_buffer + capacity);
  setp(m_buffer + capacity, m_buffer + doublecapacity);
}

buffer::buffer(buffer&& rhs)
  : m_buffer(nullptr)
{
  *this = std::move(rhs);
}

buffer::~buffer()
{
  if (m_buffer)
    eve::allocator::global().deallocate(eve_here, m_buffer);
}

bool buffer::try_fill()
{
  validate();

  if (gptr() < egptr())
    return true;

  auto capacity = (epptr() - eback()) >> 1;
  eve::size bytes = eve::size(capacity);
  if (!m_socket->try_receive(eback(), bytes))
    return false;
  setg(eback(), eback(), eback() + bytes);
  return true;
}

buffer& buffer::operator=(buffer&& rhs)
{
  if (m_buffer)
    eve::allocator::global().deallocate(eve_here, m_buffer);

  m_socket = rhs.m_socket;
  m_buffer = rhs.m_buffer;
  setp(rhs.pbase(), rhs.epptr());
  pbump(int(rhs.pptr() - rhs.pbase()));
  setg(rhs.eback(), rhs.gptr(), rhs.egptr());

  rhs.m_socket = nullptr;
  rhs.m_buffer = nullptr;
  rhs.setp(nullptr, nullptr);
  rhs.setg(nullptr, nullptr, nullptr);

  return *this;
}

int buffer::sync()
{
  validate();

  if (pbase() < pptr())
  {
    m_socket->send_all(pbase(), int(pptr() - pbase()));
    setp(pbase(), epptr());
  }
  return 0;
}

buffer::int_type buffer::underflow()
{
  validate();
  auto capacity = (epptr() - eback()) >> 1;
  auto bytes = m_socket->receive(eback(), capacity);
  setg(eback(), eback(), eback() + bytes);
  return *eback();
}

buffer::int_type buffer::overflow(int_type meta)
{
  validate();

  if (traits::eq_int_type(traits::eof(), meta))
    return traits::not_eof(meta);

  sync();

  *pptr() = meta;
  pbump(1);

  return meta;
}

void buffer::validate() const
{
  if (!m_socket)
    throw std::runtime_error("Network buffer has no socket associated to.");
}
