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

  m_buffer = (char*)eve::allocator::global().allocate(eve_here, capacity, 1);
  setg(m_buffer + capacity, m_buffer + doublecapacity, m_buffer + doublecapacity);
  setp(m_buffer, m_buffer + capacity);
}

buffer::~buffer()
{
  if (m_buffer)
    eve::allocator::global().deallocate(eve_here, m_buffer);
}

int buffer::sync()
{
  if (pbase() < pptr())
  {
    m_socket->send_all(pbase(), pptr() - pbase());
    setp(pbase(), epptr());
  }
  return 0;
}

buffer::int_type buffer::underflow()
{
  auto bytes = m_socket->receive(eback(), gptr() - eback());
  setg(eback(), eback(), eback() + bytes);
  return *eback();
}

buffer::int_type buffer::overflow(int_type meta)
{
  if (traits::eq_int_type(traits::eof(), meta))
    return traits::not_eof(meta);

  sync();

  *pptr() = meta;
  pbump(1);

  return meta;

}
