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
#include <eve/debug.h>
#include <eve/storage.h>

TEST(Lib, debug)
{
  eve_assert(2 > 1);
}

TEST(Lib, storage)
{
  using namespace eve::storage;

  fixed<7, 1> data1;
  
  (void)data1;
  EXPECT_EQ(7, sizeof(data1));

  fixed<8, 4> data4;

  EXPECT_EQ(0, (eve::u64)&data4 % 4);
  EXPECT_EQ(8, sizeof(data4));

  fixed<32, 16> data16;
 
  EXPECT_EQ(0, (eve::u64)&data16 % 16);
  EXPECT_EQ(32, sizeof(data16));

  struct Foo
  {
    int value;
    Foo(int val) : value(val) { }
    ~Foo() { }
  };

  auto foo = create<Foo>(data16, 42);
  EXPECT_EQ(42, foo->value);
  destroy(foo);

  dynamic<1, eve_alignof(Foo)> dynstorage;

  struct Bar
  {
    int value;
    char dummy[100];
    Bar(int val) : value(val) { }
    ~Bar() { }
  };

  auto bar = eve::storage::create<Bar>(dynstorage, 42);
  EXPECT_EQ(42, bar->value);
  EXPECT_TRUE(dynstorage.exceeds());
  eve::storage::destroy(bar);
}

TEST(Lib, allocator)
{
  eve::allocator::heap h;
  eve::allocator::any a(&h);
}
