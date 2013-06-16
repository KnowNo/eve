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
#include <eve/resource.h>

struct dummy_res : public eve::deserializable_resource<dummy_res>
{
public:
  dummy_res(int param)
    : param(param) {}
  
  int param;
  std::string name;

  eve_serializable(dummy_res, name);
};

struct dummy_host : public eve::deserializable_resource<dummy_host>
{
public:
  dummy_host()
    : dummy((eve::resource_host*)this, 3)
  {
  }

  int value;
  eve::resource::ptr<dummy_res, int> dummy;

  eve_serializable(dummy_host, value, dummy);
};

TEST(Application, application)
{
  eve::resource::ptr<dummy_host> host;
  host.load("data/dummy_host.txt");
  
  eve::resource::ptr<dummy_res, int> res(3);
  res.load("data/dummy_res.txt");

  res.force_reload();

  EXPECT_EQ("Foo", host->dummy->name);
}

