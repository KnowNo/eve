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

#include "eve/net/socket.h"

#ifdef EVE_WINDOWS

#include <WinSock2.h>

// Namespace used for wrapping system functions in order to avoid naming collision
inline SOCKET sys_socket(int a, int b, int c) { return socket(a, b, c); }
inline int sys_listen(SOCKET socket, int conns) { return listen(socket, conns); }
inline SOCKET sys_accept(SOCKET socket, sockaddr* addr, int* len) { return accept(socket, addr, len); }
inline int sys_connect(SOCKET s, sockaddr* name, int namelen) { return connect(s, name, namelen); }
inline int sys_send(SOCKET s, const char* buf, int len, int flags) { return send(s, buf, len, flags); }
inline int sys_recv(SOCKET s, char* buf, int len, int flags) { return recv(s, buf, len, flags); }
inline int sys_shutdown(SOCKET s, int how) { return shutdown(s, how); }

////////////////////////////////////////////////////////////////////////////////////////////////////

static WSADATA s_WSAdata;

void initialize_net()
{
  WSAStartup(MAKEWORD(2, 0), &s_WSAdata);
}

void terminate_net()
{
  WSACleanup();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eve::socket::address::address(domain domain)
{
  m_pimpl.construct<sockaddr_in>(eve_here);
  initialize(domain); 
}

eve::socket::address::address(int port, domain domain)
{
  m_pimpl.construct<sockaddr_in>(eve_here);
  set(port, domain);
}

eve::socket::address::address(const std::string& hostname, int port, domain domain)
{
  m_pimpl.construct<sockaddr_in>(eve_here);
  set(hostname, port, domain);
}

eve::socket::address::address(const address& rhs)
{
  m_domain = rhs.m_domain;
  m_pimpl.construct<sockaddr_in>(eve_here);
  memcpy(&m_pimpl, rhs.m_pimpl, sizeof(sockaddr_in));
}

eve::socket::address::~address()
{
  eve::destroy<sockaddr_in>(eve_here, &m_pimpl.as<sockaddr_in>());
}

void eve::socket::address::set(int port, domain domain)
{
  initialize(domain);

  auto& addr = m_pimpl.as<sockaddr_in>(); 
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);
}

void eve::socket::address::set(const std::string& hostname, int port, domain domain)
{
  set(port, domain);

  auto& addr = m_pimpl.as<sockaddr_in>(); 
  // resolve hostname
  hostent* he;
  if ( (he = gethostbyname(hostname.c_str()) ) == NULL )
    throw eve::socket_error("Could not resolve hostname '" + hostname + "'.");

  // copy the network address to sockaddr_in structure
  memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);
}

eve::socket::address& eve::socket::address::operator=(const address& rhs)
{
  m_domain = rhs.m_domain;
  memcpy(&m_pimpl, rhs.m_pimpl, sizeof(sockaddr_in));
  return *this;
}

void eve::socket::address::initialize(domain domain)
{
  m_domain = domain;
  auto& addr = m_pimpl.as<sockaddr_in>(); 
  memset(&addr, 0, sizeof(sockaddr_in));
  addr.sin_family = m_domain == domain::IPv4 ? AF_INET : AF_INET6;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eve::socket::socket()
  : m_state(state::invalid)
  , m_address(domain::IPv4)
  , m_blocking(true)
{
  m_pimpl.as<SOCKET>() = 0;
}

eve::socket::socket(type type, domain domain)
  : socket()
{
  create(type, domain);
}

eve::socket::socket(socket&& rhs)
  : m_address(rhs.m_address.m_domain)
{
  m_type = rhs.m_type;
  m_state = rhs.m_state;
  m_blocking = rhs.m_blocking;
  m_pimpl.as<SOCKET>() = rhs.m_pimpl.as<SOCKET>();
  rhs.m_pimpl.as<SOCKET>() = 0;
  rhs.m_state = state::invalid;
}

eve::socket::~socket()
{
  if (m_state != state::invalid)
  {
    eve_assert(m_pimpl.as<SOCKET>() != 0);
    shutdown();
    closesocket(m_pimpl.as<SOCKET>());
  }
}

void eve::socket::create(type type, domain domain)
{
  if (m_pimpl.as<SOCKET>() != 0)
    throw socket_error("Socket already created.");

  m_address = address(domain);

  m_pimpl.as<SOCKET>() = sys_socket(
    domain == domain::IPv4 ? AF_INET : AF_INET6,
    type == type::stream ? SOCK_STREAM : SOCK_DGRAM,
    0
  );

  if (m_pimpl.as<SOCKET>() == INVALID_SOCKET)
    throw eve::socket_error("Could not create new socket.", WSAGetLastError());

  m_type = type;
  m_state = state::closed;
}

void eve::socket::listen(eve::uint32 port, eve::size backlog)
{
  if (m_state != state::closed)
    throw eve::socket_error("Cannot make socket listen, not closed.");

  make_blocking(true);
  m_address.set(port, m_address.m_domain);

  auto& sa = m_address.m_pimpl.as<sockaddr_in>();

  if (bind(m_pimpl.as<SOCKET>(), (sockaddr*)&sa, sizeof(address)) < 0)
    throw eve::socket_error("Cannot bind socket to address.", WSAGetLastError());

  if (sys_listen(m_pimpl.as<SOCKET>(), backlog))
    throw eve::socket_error("Cannot set socket in listen mode.", WSAGetLastError());

  m_state = state::listening;
}

eve::socket eve::socket::accept()
{
  socket client;
  do_accept(client, true);
  return client;
}

void eve::socket::connect(const address& address)
{
  do_connect(address, true);
}

eve::size eve::socket::send(const char* data, eve::size size)
{
  do_send(data, size, true);
  return size;
}

void eve::socket::send_all(const char* data, eve::size size)
{
  const char* ptr = data;
  const char* end = data + size;
  while (ptr < end)
    ptr += send(ptr, eve::size(end - ptr));
}

eve::size eve::socket::receive(char* buffer, eve::size size)
{
  do_receive(buffer, size, true);
  return size;
}

void eve::socket::receive_all(char* buffer, eve::size size)
{
  char* ptr = buffer;
  char* end = buffer + size;
  while (ptr < end)
    ptr += receive(ptr, eve::size(end - ptr));
}

bool eve::socket::try_accept(socket& client)
{
  return do_accept(client, false);
}

bool eve::socket::try_send(const char* data, eve::size& size)
{
  return do_send(data, size, false);
}

bool eve::socket::try_receive(char* buffer, eve::size& size)
{
  return do_receive(buffer, size, false);
}

void eve::socket::shutdown()
{
  if (m_state == state::invalid)
    return;

  if (m_state != state::closed)
  {
    sys_shutdown(m_pimpl.as<SOCKET>(), 2);
    m_state = state::closed;
  }
}

eve::socket& eve::socket::operator=(socket&& rhs)
{
  shutdown();
  m_type = rhs.m_type;
  m_state = rhs.m_state;
  m_address = rhs.m_address;
  m_pimpl.as<SOCKET>() = rhs.m_pimpl.as<SOCKET>();
  rhs.m_state = state::invalid;
  rhs.m_pimpl.as<SOCKET>() = 0;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool eve::socket::do_connect(const address& address, bool block)
{
  if (m_state != state::closed)
    throw eve::socket_error("Cannot connect socket, not closed.");
  
  make_blocking(block);
  auto& sock = m_pimpl.as<SOCKET>();

  m_address = address;
  auto result = sys_connect(sock, &m_address.m_pimpl.as<sockaddr>(), sizeof(sockaddr_in));
  
  if (check_result(result, block, "An error occurred while connecting socket."))
    return false;

  m_state = state::connected;
  return true;
}

bool eve::socket::do_accept(socket& client, bool block)
{
  if (m_state != state::listening)
    throw eve::socket_error("Cannot connect socket, not in listening state.");

  make_blocking(block);
  auto& sock = m_pimpl.as<SOCKET>();

  int size = sizeof(sockaddr_in);
  SOCKET& client_sock = client.m_pimpl.as<SOCKET>();
  client_sock = sys_accept(
    sock, 
    &client.m_address.m_pimpl.as<sockaddr>(),
    &size);

  if (check_result(int(client_sock), block, "An error occurred while accepting new clients."))
    return false;

  client.m_type = m_type;
  client.m_state = state::connected;
  m_address.m_domain = m_address.m_domain;
  m_blocking = false;
  return true;
}

bool eve::socket::do_send(const char* data, eve::size& size, bool block)
{
  if (m_state != state::connected)
    throw eve::socket_error("Cannot send data through socket, not connected.");
  
  make_blocking(block);
  auto& sock = m_pimpl.as<SOCKET>();

  auto result = sys_send(sock, data, size, 0);
  if (check_result(result, block, "An error occurred while sending data."))
    return false;

  size = result;
  return true;
}

bool eve::socket::do_receive(char* buffer, eve::size& size, bool block)
{
  if (m_state != state::connected)
    throw eve::socket_error("Cannot receive data through socket, not connected.");
  
  make_blocking(block);
  auto& sock = m_pimpl.as<SOCKET>();

  auto result = sys_recv(sock, buffer, size, 0);
  if (check_result(result, block, "An error occurred while receiving data."))
    return false;

  size = result;
  return true;
}

bool eve::socket::check_result(int result, bool block, const std::string& errormsg)
{
  if (result == SOCKET_ERROR)
  {
    auto error = WSAGetLastError();
    eve_assert(!(block && error == WSAEWOULDBLOCK));
    if (error == WSAEWOULDBLOCK)
      return true;
    else
      throw eve::socket_error("An error occurred while accepting new clients.", WSAGetLastError());
  }
  return false;
}

void eve::socket::make_blocking(bool blocking)
{
  if (m_blocking == blocking)
    return;

  m_blocking = blocking;
  u_long nNoBlock = u_long(!blocking);
  ioctlsocket(m_pimpl.as<SOCKET>(), FIONBIO, &nNoBlock);
}

#endif

