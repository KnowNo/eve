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

#include "vertex.h"
#include "storage.h"

/** \addtogroup Lib
  * @{
  */

namespace eve {

// TODO move this somewhere else
enum class primitive_type
{
  POINTS, LINES, LINE_LOOP, TRIANGLES, TRIANGLE_STRIP
};

// Forward declarations
namespace hwarray {
  class indices;
  template <typename> class vertices;
}

class vertexarray : uncopyable
{
public:
  static const eve::size k_max_buffers = 8;

  static void unbind();

  vertexarray();
  ~vertexarray();

  void attach(hwarray::indices* indices);

  template <typename T>
  void attach(eve::size location, hwarray::vertices<T>* vertices)
  {
    bind();
    vertices->buffer()->bind();
    do_attach(location, eve_sizeof(T), eve::vertex_info<T>::components);
  }

  void destroy();

  void draw(primitive_type primitive) const;
  void draw(primitive_type primitive, size first, size count) const;

private:
  void bind() const;
  void do_attach(size location, size stride, const eve::vertex_component* components);

  hwarray::indices* m_indices;
  fixed_storage<eve_sizeof(void*), eve_alignof(void*)> m_pimpl;
};

} // eve

/** @} */
