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
#include <eve/window.h>
#include <eve/hwbuffer.h>
#include <eve/vertexarray.h>
#include <eve/time.h>

#define GLEW_STATIC
#include <GL/glew.h>

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
  eve::application app(eve::application::module::memory_debugger);

  eve::resource::ptr<dummy_host> host;
  host.load("data/dummy_host.txt");
  
  eve::resource::ptr<dummy_res, int> res(3);
  res.load("data/dummy_res.txt");

  res.force_reload();

  EXPECT_EQ("Foo", host->dummy->name);
}

TEST(Application, window)
{
  eve::application app(eve::application::module::graphics | eve::application::module::memory_debugger);

  eve::window::config c;
  c.width = 800;
  c.height = 400;

  eve::window window("eve window test");

  window.configure(c);
  window.open();

  eve::hwbuffer buff(eve::hwbuffer::type::VERTEX);

  const eve::vec2 triangle[] =
  {
    eve::vec2(0.f, 0.f),
    eve::vec2(1.0f, 0.0f),
    eve::vec2(0.f, 1.0f)
  };

  buff.create(eve::hwbuffer::usage::STATIC_DRAW, eve_sizeof(triangle), triangle);
  
  eve::hwarray::vertices<eve::vec2> vertices(&buff);

  eve::vertexarray va;

  va.attach(0, &vertices);
  
  eve::window::event e;
  eve::stopwatch sw;
  eve::fpscounter fps;
  while (window.opened())
  {
    while (window.poll(e))
    {
      if (e.type == e.QUIT)
        return;
    }
    window.activate();

    glViewport(0, 0, window.width(), window.height());

    glColor3f(1,1,1);
    va.draw(eve::primitive_type::TRIANGLES, 0, 3);

    eve::time::fps_wait((float)sw.elapsed(), 60);
    if (fps.tick())
    {
      std::stringstream ss;
      ss << "FPS: " << fps.value();
      window.title(ss.str());
    }
    window.display();
    sw.reset();
  }
}
