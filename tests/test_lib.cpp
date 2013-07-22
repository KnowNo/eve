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
#include <eve/allocator.h>
#include <eve/application.h>
#include <eve/path.h>
#include <eve/binary.h>
#include <eve/callstack.h>
#include <eve/log.h>
#include <sstream>
#include <fstream>

struct Foo
{
  int value;
  Foo() : value(0) {}
  Foo(int val) : value(val) { }
  ~Foo() { }
};

TEST(Lib, debug)
{
  eve_assert(2 > 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TEST(Lib, log)
{
  eve::log::info("Hello this is an info message.");
  eve::log::warning("Hello this is a warning message.");
  eve::log::error("Hello this is an error message.");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TEST(Lib, storage)
{
  eve::application app(eve::application::module::memory_debugger);

  eve::fixed_storage<7, 1> data1;
  
  (void)data1;
  EXPECT_EQ(7, sizeof(data1));

  eve::fixed_storage<8, 4> data4;

  EXPECT_EQ(0, (eve::uint64)&data4 % 4);
  EXPECT_EQ(8, sizeof(data4));

  eve::fixed_storage<32, 16> data16;
 
  EXPECT_EQ(0, (eve::uint64)&data16 % 16);
  EXPECT_EQ(32, sizeof(data16));


  auto foo = eve::storage::create<Foo>(data16, 42);
  EXPECT_EQ(42, foo->value);
  eve::destruct(foo);

  eve::dyn_storage<1, eve_alignof(Foo)> dynstorage;

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
  eve::destruct(bar);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TEST(Lib, allocator)
{
  eve::application app(eve::application::module::memory_debugger);

  eve::allocator::heap h;
  eve::allocator::any a(&h);

  auto foo = eve_new Foo(33);
  foo->value = 10;
  eve::destroy(foo);

  {
    auto arr = new(eve::allocator::any(&eve::allocator::global())) int[10];
    eve::destroy_array(arr);
  }

  {
    auto arr = new(eve::allocator::any(&eve::allocator::global())) Foo[10];
    eve::destroy_array(arr);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TEST(Lib, path)
{
  eve::application app(eve::application::module::memory_debugger);

  std::string path = "C:/Foo/file.txt";

  eve::path::pop(path);
  eve::path::push(path, "\\file2.txt");
  
  auto filename = eve::path::pop(path);
  EXPECT_EQ("file2.txt", filename);
  EXPECT_EQ("C:/Foo", path);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TEST(Lib, binary)
{
  eve::application app(eve::application::module::memory_debugger);

  std::stringstream ss;

  eve::binarywriter bw(ss.rdbuf());

  bw << 3.14f << "hello";

  eve::binaryreader br(ss.rdbuf());

  float f;
  std::string str;
  br >> f >> str;

  EXPECT_FLOAT_EQ(3.14f, f);
  EXPECT_EQ("hello", str);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eve::callstack* foo()
{
  return eve_new eve::callstack;
}

TEST(Lib, callstack)
{
  eve::application app(eve::application::module::memory_debugger);

  auto pcs = eve::unique_ptr<eve::callstack>::type(foo());
  
  for (eve::size i = 0; i < pcs->size(); ++i)
  {
    auto symbol = pcs->fetch(i);
    std::cout << i << ") " << symbol.function() << " " << symbol.file() << ": " << symbol.line() << '\n';
  }
}

//TEST(Lib, string)
//{
//  eve::string hello = "Hello";
//  eve::string msg = hello + " " + "world";
//  EXPECT_STREQ("Hello world", msg.data());
//}
