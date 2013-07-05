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

#include "storage.h"
#include <string>

/** \addtogroup Net
  * @{
  */

namespace eve {

class socket : uncopyable
{
public:
  enum class type : uint8
  {
    stream,
    datagram
  };

  enum class domain: uint8
  {
    IPv4,
    IPv6
  };

  typedef enum class state : uint8
  {
    invalid,
    closed,
    listening,
    connecting,
    connected,
    connection_failed
  } state_;

  class address
  {
  public:
    address(domain domain);
    address(int port, domain domain = domain::IPv4);
    address(const std::string& hostname, int port, domain domain = domain::IPv4);

    void set(int port, domain domain = domain::IPv4);
    void set(const std::string& hostname, int port, domain domain = domain::IPv4);

  private:
    void initialize(domain domain);

    domain m_domain;
    fixed_storage<24> m_pimpl;
    friend class socket;
  };

  socket();
  socket(type type, domain domain = domain::IPv4);
  socket(socket&& rhs);
  ~socket();

  state state() const { return m_state; }

  /** Makes this socket listen for connections at port @p and any address.
      @param backlog is the maximum number of pending connections waiting to be accepted. */
  void listen(eve::uint32 port, eve::size backlog);
  socket accept();
  void connect(const std::string& host, uint32 port);

  bool try_accept(socket& client);


  void close();
  
  socket& operator=(socket&& rhs);

private:
  bool do_accept(socket& client, bool block);
  void make_blocking(bool blocking);
  
  type m_type;
  state_ m_state;
  address m_address;
  bool m_blocking;
  fixed_storage<eve_sizeof(void*), eve_alignof(void*)> m_pimpl;
};

} // eve

/** }@ */
