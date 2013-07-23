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

#include "eve/vertexarray.h"
#include "eve/exceptions.h"
#include "eve/hwbuffer.h"

#define GLEW_STATIC
#include <GL/glew.h>

using namespace eve;

static const GLenum k_gl_type[] = { GL_ELEMENT_ARRAY_BUFFER, GL_ARRAY_BUFFER, GL_UNIFORM_BUFFER };

void vertexarray::unbind()
{
  glBindVertexArray(0);
}

vertexarray::vertexarray()
  : m_id(0)
  , m_indices(nullptr)
{
  glGenVertexArrays(1, &m_id);
  if (m_id == 0)
    throw eve::system_error("Cannot create the vertex array.");
}

vertexarray::~vertexarray()
{
  destroy();
}

void vertexarray::attach(hwarray::indices* indices)
{
  m_indices = indices;
}

void vertexarray::destroy()
{
  if (m_id != 0)
  {
    glDeleteVertexArrays(0, &m_id);
    m_id = 0;
  }
}

void vertexarray::draw(primitive_type primitive) const
{
  if (m_indices)
  {
    if (m_indices->size() == 0)
      return;
    draw(primitive, 0, m_indices->size());
  } else
  {

  }
}

void vertexarray::draw(primitive_type primitive, size first, size count) const
{
  static const GLenum k_gl_primitives[] =
  {
    GL_POINTS, GL_LINES, GL_LINE_LOOP, GL_TRIANGLES, GL_TRIANGLE_STRIP
  };

  static const GLenum k_gl_indextype[] =
  {
    GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT
  };

  bind();

  if (m_indices)
  {
    // Make use of the index buffer
    m_indices->buffer()->bind();
    
    glDrawElements(
      k_gl_primitives[size(primitive)],
      count,
      k_gl_indextype[size(m_indices->indextype())],
      nullptr
    );
    
    hwbuffer::unbind(hwbuffer::type::index);
  
  } else
    glDrawArrays(k_gl_primitives[int(primitive)], first, count);
  
  unbind();
}

void vertexarray::bind() const
{
  glBindVertexArray(m_id);
}

void vertexarray::do_attach(const hwbuffer* buffer, size location, size stride, const eve::vertex_component* components)
{
  static const GLenum k_gl_primitives[] = 
  {
    GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, GL_UNSIGNED_INT, 0, 0, GL_FLOAT, GL_DOUBLE
  };

  bind();
  buffer->bind();

  eve::size index = 0;
  eve::size offset = 0;

  while (components[index].count != 0)
  {
    glVertexAttribPointer(location, components[index].count, k_gl_primitives[size(components[index].type)], false, stride, (const void*)offset);
    glEnableVertexAttribArray(location);
    offset += components[index].count * eve::arithmetic_type_size(components[index].type);
    ++location;
    ++index;
  }

  unbind();
}