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
#include "gfxdevice.h"
#include "storage.h"
#include "uncopyable.h"

/** \addtogroup Gfx
  * @{
  */

namespace eve {

class hwbuffer : eve::uncopyable
{
public:
  typedef enum class type
  {
    index, vertex, uniform
  } type_t;

  enum class usage
  {
    static_draw, dynamic_draw, stream_draw, dynamic_copy
  };

  /** Makes no buffer of type @p type in use. */
  static void unbind(type type);

  hwbuffer(type type);
  ~hwbuffer();

  type_t type() const { return m_type; }
  eve::size capacity() const { return m_capacity; }

  void create(usage usage, eve::size capacity, const void* data = nullptr);
  void destroy();

  /** Sets this buffer in use. */
  void bind() const;

  /** Binds this buffer and writes @p count bytes at @p data to the buffer starting
      from byte @p offset. */
  void write(size offset, const void* data, size count);

private:
  eve::id m_id;
  type_t m_type;
  eve::size m_capacity;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

namespace hwarray {

class base
{
public:
  base();
  const hwbuffer* buffer() const { return m_buffer; }
  
  eve::size offset() const { return m_offset; }
  eve::size size() const { return m_size; }
  eve::size capacity() const { return m_capacity; }
  bool instanced() const { return m_instanced; }

  void clear() { m_size = 0; }

protected:
  void setup(hwbuffer* buffer, eve::size element_size, eve::size offset, eve::size capacity, bool instanced);
  void setup(hwbuffer* buffer, eve::size element_size, bool instanced);
  void write(const eve::size element_size, const void* data, eve::size first, eve::size size);

private:
  hwbuffer* m_buffer;
  eve::size m_offset;
  eve::size m_size;
  eve::size m_capacity;
  bool m_instanced;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class indices : public hwarray::base
{
public:
  enum class type
  {
    UBYTE, USHORT, UINT
  };

  indices();
  indices(hwbuffer* buffer, type type, bool instanced = false);
  indices(hwbuffer* buffer, type type, eve::size offset, eve::size capacity, bool instanced = false);
  
  type indextype() const { return m_type; }

  void setup(hwbuffer* buffer, type type, bool instanced = false);
  void setup(hwbuffer* buffer, type type, eve::size offset, eve::size capacity, bool instanced = false);

  void write(const void* data, eve::size first, eve::size size);

private:
  type m_type;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename VertexType>
class vertices : public hwarray::base
{
public:
  vertices();
  vertices(hwbuffer* buffer, bool instanced = false);
  vertices(hwbuffer* buffer, eve::size offset, eve::size capacity, bool instanced = false);

  void setup(hwbuffer* buffer, bool instanced = false);
  void setup(hwbuffer* buffer, eve::size offset, eve::size capacity, bool instanced = false);

  void write(const VertexType* data, eve::size first, eve::size size);
};

} // hwarray
} // eve

#include "detail/hwbuffer.inl"

/** @} */
