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

#include <gtest/gtest.h>
#include <eve/application.h>
#include <eve/net/socket.h>
#include <eve/net/buffer.h>
#include <eve/binary.h>

TEST(Net, SocketAndBuffer)
{
  eve::application app(eve::application::module::networking | eve::application::module::memory_debugger);

  eve::socket server(eve::socket::type::stream);
  server.listen(10000, 4);

  eve::socket client(eve::socket::type::stream);
  client.connect(eve::socket::address("localhost", 10000));

  auto peer = server.accept();

  {
    eve::net::buffer buf(&peer, 2);
    eve::binarywriter bw(&buf);
    bw << 3.14f;
  }

  float data;

  {
    eve::net::buffer buf(&client, 2);
    eve::binaryreader br(&buf);
    br >> data;
  }

  EXPECT_FLOAT_EQ(3.14f, data);
}
