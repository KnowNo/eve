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
#include <eve/serialization/vector.h>
#include <eve/serialization.h>

TEST(Lib, serialization)
{
  eve::application app(eve::application::module::memory_debugger);

  struct Boo
  {
    int j;
    Boo() { }
    Boo(int j) : j(j) {}
    eve_serializable(Boo, j);
  };

  struct Foo
  {
    int i;
    float f;
    double d;
    std::vector<Boo> boos;
    eve_serializable(Foo, i, f, d, boos);
  };

  Foo foo;
  foo.i = 42;
  foo.f = 3.14f;
  foo.d = 1.41;
  foo.boos.emplace_back(11);
  
  std::stringstream ss;
  eve::serialize_as_text(foo, ss);

  foo.i = 0;
  foo.f = 0;
  foo.d = 0;
  foo.boos.clear();

  eve::deserialize_as_text(ss, foo);

  EXPECT_EQ(42, foo.i);
  EXPECT_FLOAT_EQ(3.14f, foo.f);
  EXPECT_DOUBLE_EQ(1.41, foo.d);
  EXPECT_EQ(11, foo.boos[0].j);
}
