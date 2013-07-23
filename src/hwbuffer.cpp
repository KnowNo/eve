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

#include "eve/hwbuffer.h"
#include "eve/exceptions.h"

#define GLEW_STATIC
#include <GL/glew.h>

using namespace eve;

static const GLenum k_gl_buffer_type[] = { GL_ELEMENT_ARRAY_BUFFER, GL_ARRAY_BUFFER, GL_UNIFORM_BUFFER };
static const GLenum k_gl_indextype[] = { GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT };
static const GLenum k_gl_usage[] = { GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW, GL_DYNAMIC_COPY };

void hwbuffer::unbind(type_t type)
{
  glBindBuffer(k_gl_buffer_type[size(type)], 0);
}

hwbuffer::hwbuffer(type_t type)
  : m_id(0)
  , m_type(type)
  , m_capacity(0)
{
}

hwbuffer::~hwbuffer()
{
  destroy();
}

void hwbuffer::create(usage usage, eve::size capacity, const void* data)
{
  if (m_id != 0)
    throw eve::system_error("Hardware buffer already created.");

  glGenBuffers(1, &m_id);

  if (m_id == 0)
    throw eve::system_error("Cannot create a new GL buffer.");

  m_capacity = capacity;

  bind();

  glBufferData(k_gl_buffer_type[size(m_type)], m_capacity, data, k_gl_usage[size(usage)]);
  
  unbind(m_type);
}

void hwbuffer::destroy()
{
  if (m_id != 0)
  {
    glDeleteBuffers(1, &m_id);
    m_id = 0;
  }
}

void hwbuffer::bind() const
{
  glBindBuffer(k_gl_buffer_type[size(m_type)], m_id);
}

void hwbuffer::write(size offset, const void* data, size count)
{
  eve_assert(offset + count <= m_capacity);
  bind();
  glBufferSubData(k_gl_buffer_type[size(m_type)], offset, count, data);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

hwarray::base::base()
  : m_buffer(nullptr), m_offset(0), m_size(0), m_capacity(0), m_instanced(false)
{
}

void hwarray::base::setup(hwbuffer* buffer, eve::size element_size, eve::size offset, eve::size capacity, bool instanced)
{
  eve_assert(buffer);
  eve_assert(offset + capacity * element_size <= buffer->capacity());
  m_buffer = buffer;
  m_offset = offset;
  m_capacity = capacity;
  m_instanced = instanced;
}

void hwarray::base::setup(hwbuffer* buffer, eve::size element_size, bool instanced)
{
  eve_assert(buffer);
  setup(buffer, element_size, 0, buffer->capacity() / element_size, instanced);
}

void hwarray::base::write(const eve::size element_size, const void* data, eve::size first, eve::size size)
{
  eve_assert(first + size <= m_capacity);
  m_buffer->write(m_offset + first * element_size, data, size * element_size);
  m_size = std::max(m_size, first + size);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static const size k_index_type_size[] =
{
  eve_sizeof(unsigned char), eve_sizeof(unsigned short), eve_sizeof(unsigned int)
};

hwarray::indices::indices()
{
}

hwarray::indices::indices(hwbuffer* buffer, type type, bool instanced)
{
  setup(buffer, type, instanced);
}

hwarray::indices::indices(hwbuffer* buffer, type type, eve::size offset, eve::size capacity, bool instanced)
{
  setup(buffer, type, offset, capacity, instanced);
}

void hwarray::indices::setup(hwbuffer* buffer, type type, bool instanced)
{
  m_type = type;
  base::setup(buffer, k_index_type_size[eve::size(type)], instanced);
}

void hwarray::indices::setup(hwbuffer* buffer, type type, eve::size offset, eve::size capacity, bool instanced)
{
  m_type = type;
  base::setup(buffer, k_index_type_size[eve::size(type)], offset, capacity, instanced);
}

void hwarray::indices::write(const void* data, eve::size first, const eve::size size)
{
  base::write(k_index_type_size[eve::size(m_type)], data, first, size);
}
