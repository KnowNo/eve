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

#include "eve/storage.h"
#include "eve/exceptions.h"
#include <string>

/** \addtogroup Net
  * @{
  */

namespace eve {

/** Error potentially thrown by any eve::socket method. */
class socket_error : public eve::system_error
{
public:
  socket_error(const std::string& message) throw () : eve::system_error(message) { }
  socket_error(const std::string& message, int errorID) throw () : eve::system_error(message, errorID) {}
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/** This class provides a thin platform-independent abstraction layer over system sockets
    used for networking. */
class socket : private uncopyable
{
public:
  enum class type : uint8
  {
    /** Used for reliable, stream oriented, connection-based sockets (TCP). */
    stream,

    /** Used for unreliable, packet oriented, connection-less sockets (UDP). */
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
    connected,
  } state_;

  /** This class provides a platform- and protocol-independent abstraction of an IP address. */
  class address
  {
  public:
    address(domain domain = domain::IPv4);
    address(int port, domain domain = domain::IPv4);
    address(const std::string& hostname, int port, domain domain = domain::IPv4);
    address(const address& rhs);
    ~address();

    /** Sets this address to any address and port @p port. */
    void set(int port, domain domain = domain::IPv4);
    
    /** Sets this address to hostname @p hostname and port @p port. */
    void set(const std::string& hostname, int port, domain domain = domain::IPv4);
    
    address& operator=(const address& rhs);

  private:
    void initialize(domain domain);

    domain m_domain;
    fixed_storage<16> m_pimpl;
    friend class socket;
  };

public:

  /** After constructing the socket call create() for creating it. */
  socket();
  
  /** Creates a socket of specified @p type and @p domain. After constructing it is ready to be used. */
  socket(type type, domain domain = domain::IPv4);
  
  socket(socket&& rhs);
  ~socket();

  /** @returns the current state of the socket. */
  state state() const { return m_state; }

  /** Creates a new socket. If already created throws a socket_error. */
  void create(type type = type::stream, domain domain = domain::IPv4);

  /** Makes this socket listen for connections at port @p and any address.
      @param backlog is the maximum number of pending connections waiting to be accepted.
      @note On error or timeout it throws a socket_error. */
  void listen(eve::uint32 port, eve::size backlog);

  /** Blocks until a new socket connects.
      @note On error or timeout it throws a socket_error. */
  socket accept();

  /** Blocks until it connects to the socket listening at @p address.
      @note On error or timeout it throws a socket_error. */
  void connect(const address& address);

  /** Blocks until some of @p data has been sent.
      @param data buffer of bytes containing the data to be sent.
      @param size the size in bytes of the data to be sent.
      @returns the actual number of bytes sent that could be < @p size.
      @note On error or timeout it throws a socket_error. */
  eve::size send(const char* data, eve::size size);

  /** Blocks until all of @p data has been sent.
      @param data buffer of bytes containing the data to be sent.
      @param size the size in bytes of the data to be sent.
      @note On error or timeout it throws a socket_error. */
  void send_all(const char* data, eve::size size);

  /** Blocks until some data has been received.
      @param buffer target buffer that will hold received data.
      @param size the size in bytes of the data to be received.
      @returns the actual number of bytes received that could be < @p size.
      @note On error or timeout it throws a socket_error. */
  eve::size receive(char* buffer, eve::size size);

  /** Blocks until all data has been received.
      @param buffer target buffer that will hold received data.
      @param size the size in bytes of the data to be received.
      @note On error or timeout it throws a socket_error. */
  void receive_all(char* buffer, eve::size size);
  
  /** Tries to accept a connection without blocking the thread.
      @param client will be set to the connected peer. Valid only if return true.
      @returns true if some peer actually connected, false if operation would block.
      @note On error or timeout it throws a socket_error. */
  bool try_accept(socket& client);

  /** Tries to connect to target @p address.
      @returns true if connection was performed, false if operation would block.
      @note On error or timeout it throws a socket_error. */
  void try_connect(const address& address);
  
  /** Tries to send some @p data without blocking.
      @param data buffer of bytes containing the data to be sent.
      @param size Input: the size in bytes of the data to be sent.
                  Output: after call, if true returned, contains the number of bytes sent.
      @returns true if some data has been sent, false if operation would block.
      @note On error or timeout it throws a socket_error. */
  bool try_send(const char* data, eve::size& size);

  /** Tries to receive some data.
      @param buffer target buffer that will hold received data.
      @param size Input: the size in bytes of the data to be received.
                  Output: after call, if true returned, contains the number of bytes received.
      @returns true if some data was received, false if no data was waiting to be received.
      @note On error or timeout it throws a socket_error. */
  bool try_receive(char* buffer, eve::size& size);

  /** Shuts down the eventual connection if socket is connected.
      @note after calling this the socket will be in 'closed' state. */
  void shutdown();
  
  //// Operator overloads
  socket& operator=(socket&& rhs);

private:
  bool do_connect(const address& address, bool block);
  bool do_accept(socket& client, bool block);
  bool do_send(const char* data, eve::size& size, bool block);
  bool do_receive(char* buffer, eve::size& size, bool block);
  bool check_result(int result, bool block, const std::string& errormsg);
  void make_blocking(bool blocking);
  
  type m_type;
  state_ m_state;
  address m_address;
  bool m_blocking;
  fixed_storage<eve_sizeof(void*), eve_alignof(void*)> m_pimpl;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

} // eve

/** }@ */
