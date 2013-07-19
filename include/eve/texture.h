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

#include "math.h"
#include "storage.h"
#include "resource.h"

/**
 * \addtogroup Graphics
 * @{
 */
  
namespace eve
{

class pixelbuffer;

class texture : public eve::resource
{
public:
  typedef enum class type
  {
    TEX_2D,
    TEX_3D,
    RECTANGLE,
    ARRAY_2D,
    BUFFER
  } type_t;

  typedef enum class typeformat
  {
    UNSIGNED_BYTE,
    HALF_FLOAT,
    FLOAT,
  } typeformat_t;

  typedef enum class channels
  {
    MONO,
    DUAL,
    TRIPLE,
    QUAD
  } channels_t;

  typedef enum class filtermode
  {
    NEAREST,
    LINEAR
  } filtermode_t;

  typedef enum class wrapmode
  {
    REPEAT,
    CLAMP_TO_EDGE,
    CLAMP_TO_BORDER
  } wrapmode_t;

  static void unbind(type_t type);

  texture();
  texture(typeformat format, channels channels);
  texture(const vec2u& size, unsigned depth, type type, typeformat format, channels channels, filtermode filtermode = filtermode::LINEAR, wrapmode wrapmode = wrapmode::CLAMP_TO_EDGE, bool mipmap = false);
  ~texture();

  type_t type() const { return m_type; }
  unsigned width() const { return m_size.x; }
  unsigned height() const { return m_size.y; }
  unsigned depth() const { return m_size.z; }
  const vec2u& size() const { return m_size.xy(); }
  typeformat typeformat() const { return m_typeformat; }
  channels channels() const { return m_channels; }
  filtermode filtermode() const { return m_filtermode; }
  wrapmode wrapmode() const { return m_wrapmode; }
  bool mipmapped() const { return m_mipmapped; }

  void create(type_t type, const vec2u& size, unsigned depth, typeformat_t format, channels_t channels, filtermode_t filtermode, wrapmode_t wrapmode, bool mipmap, const void* data);
  void create(type_t type, const vec2u& size, unsigned depth, typeformat_t format, channels_t channels, filtermode_t filtermode, wrapmode_t wrapmode, bool mipmap);
  void create(type_t type, const pixelbuffer& pixelbuffer, filtermode_t filtermode, wrapmode_t wrapmode, bool mipmap);

  void write_layer(unsigned layer, const void* data);

  void activate(unsigned unit = 0) const; /* API specific */
  void destroy();

  //vec4 transform_coordinate(const vec2i& coordinate, const vec2i& size) const;

  void load(std::istream& source) override;
  void unload() override;

protected:
  void on_reload() override;

private:
  void setup(type_t type, const vec2u& size, unsigned depth, typeformat_t format, channels_t channels, filtermode_t filtermode, wrapmode_t wrapmode, bool mipmap);

  /* API specific */
  void init_id();
  bool created() const;
  void create_id();
  void write(const void* data);
  void subwrite(unsigned depth, const void* data);
  void generate_mipmaps();
  void set_filters();
  void set_clampmode();  

  fixed_storage<eve_sizeof(void*), eve_alignof(void*)> m_pimpl;
  type_t m_type;
  vec3u m_size;
  typeformat_t m_typeformat;
  channels_t m_channels;
  filtermode_t m_filtermode;
  wrapmode_t m_wrapmode;
  bool m_mipmapped;
  resource::ptr<pixelbuffer> m_pixelbuffer;

  eve_declare_serializable
};

} //eve

/** }@ */
