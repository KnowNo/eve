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

#include "eve/texture.h"
#include "eve/pixelbuffer.h"
#include "eve/storage.h"

using namespace eve;

eve_define_enum(eve::texture::type_t,
  TEX_2D, TEX_3D, RECTANGLE, ARRAY_2D, BUFFER
)

eve_define_enum(eve::texture::typeformat_t,
  UNSIGNED_BYTE, HALF_FLOAT, FLOAT
)

eve_define_enum(eve::texture::channels_t,
  MONO, DUAL, TRIPLE, QUAD
)

eve_define_enum(eve::texture::filtermode_t,
  NEAREST, LINEAR
)

eve_define_enum(eve::texture::wrapmode_t,
  REPEAT, CLAMP_TO_EDGE, CLAMP_TO_BORDER
)

eve_define_serializable_named(eve::texture,
  (m_type, "type"),
  (m_typeformat, "typeformat"),
  (m_channels, "channels"),
  (m_filtermode, "filtermode"),
  (m_wrapmode, "wrapmode"),
  (m_mipmapped, "mipmapped"),
  (m_pixelbuffer, "pixelbuffer"))

////////////////////////////////////////////////////////////////////////////////////////////////////

texture::texture()
  : m_type(type_t::TEX_2D)
  , m_typeformat(typeformat_t::UNSIGNED_BYTE)
  , m_channels(channels_t::TRIPLE)
  , m_filtermode(filtermode_t::LINEAR)
  , m_wrapmode(wrapmode_t::CLAMP_TO_EDGE)
  , m_mipmapped(false)
  , m_pixelbuffer((eve::resource_host*)this)
{
  init_id();
}

texture::texture(typeformat_t format, channels_t channels)
  : m_type(type_t::TEX_2D)
  , m_typeformat(format)
  , m_channels(channels)
  , m_filtermode(filtermode_t::LINEAR)
  , m_wrapmode(wrapmode_t::CLAMP_TO_EDGE)
  , m_mipmapped(false)
  , m_pixelbuffer((eve::resource_host*)this)
{
  create_id();
}

texture::texture(const vec2u& size, unsigned depth, type_t type, typeformat_t format, channels_t channels, filtermode_t filtermode, wrapmode_t wrapmode, bool mipmap)
  : m_pixelbuffer((eve::resource_host*)this)
{
  init_id();
  create(type, size, depth, format, channels, filtermode, wrapmode, mipmap);
}

texture::~texture()
{
  destroy();
}

void texture::create(type_t type, const vec2u& size, unsigned depth, typeformat_t format, channels_t channels, filtermode_t filtermode, wrapmode_t wrapMode, bool mipmap, const void* data)
{
  setup(type, size, depth, format, channels, filtermode, wrapMode, mipmap);
  activate();
  write(data);
  set_filters();
  set_clampmode();
}

void texture::create(type_t type, const vec2u& size, unsigned depth, typeformat_t format, channels_t channels, filtermode_t filtermode, wrapmode_t wrapMode, bool mipmap)
{
  static unsigned k_type_size[] = {sizeof(unsigned char), sizeof(float), sizeof(float)};
  auto buffersize = (static_cast<eve::size>(channels) + 1) * k_type_size[static_cast<eve::size>(format)] * size.x * size.y;
  eve::dyn_storage<128, 1> buffer;
  buffer.reserve(buffersize);
  memset(buffer, 0, buffersize);
  create(type, size, 1, format, channels, filtermode, wrapMode, mipmap, buffer);
}

void texture::create(type_t type, const pixelbuffer& pixelBuffer, filtermode_t filtermode, wrapmode_t wrapMode, bool mipmap)
{
  create(type, pixelBuffer.size(), 1, typeformat_t::UNSIGNED_BYTE, channels_t(pixelBuffer.channels() - 1), filtermode, wrapMode, mipmap, pixelBuffer.data());
}


void texture::write_layer(unsigned layer, const void* data)
{
  activate();
  subwrite(layer, data);
}

//vec4 texture::transformCoordinate(const vec2i& coordinate, const vec2i& size) const
//{
//  return vec4(
//    float(coordinate.x) / getWidth(),
//    float(getHeight() - size.y - coordinate.y) / getHeight(),
//    float(size.x) / getWidth(),
//    float(size.y) / getHeight());
//}

struct texture_info
{
  texture::type_t type;
  texture::typeformat_t typeformat;
  texture::channels_t channels;
  texture::filtermode_t filtermode;
  texture::wrapmode_t wrapmode;
  bool mipmapped;
  std::string pixelbuffer;

  texture_info()
    : type(texture::type_t::TEX_2D)
    , typeformat(texture::typeformat_t::UNSIGNED_BYTE)
    , channels(texture::channels_t::TRIPLE)
    , filtermode(texture::filtermode_t::LINEAR)
    , wrapmode(texture::wrapmode_t::CLAMP_TO_EDGE)
    , mipmapped(false)
    {
    }

  eve_serializable(texture_info,
    type, typeformat, channels, filtermode, wrapmode, mipmapped, pixelbuffer);
};

void texture::load(std::istream& source)
{
  texture_info info;
  eve::deserialize_as_text(source, info);

  switch (m_type)
  {
    case type::TEX_2D:
    case type::RECTANGLE:
    {
      std::string path = this->path();
      eve::path::pop(path);
      eve::path::push(path, info.pixelbuffer);
      m_pixelbuffer.load(path);
      create(m_type, *m_pixelbuffer, info.filtermode, info.wrapmode, info.mipmapped);
      break;
    }

    /* TODO implement this
    case Texture::Type::TEX_3D:
    case Texture::Type::ARRAY_2D:
      if (mLayers.getSize() == 0)
        BI_LOG_ERROR("No layers specified in texture array resource.")
      else
        GOD::enqueueMainSync([this]
        {
          mTexture.create(mTexture.getType(), mLayers[0]->getSize(), (unsigned)mLayers.getSize()
            , mTexture.getTypeFormat(), Texture::Channels(mLayers[0]->getChannels() - 1), mTexture.getFilterMode()
            , mTexture.getWrapMode(), mTexture.isMipmappingEnabled());
          for (unsigned i = 0; i < mLayers.getSize(); ++i)
            if (mLayers[i]->isLoaded())
              mTexture.writeLayer(i, mLayers[i]->getData());
        });
      break;*/
    
    default:
      eve_internal_error;
  }
}

void texture::unload()
{
  destroy();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void texture::on_reload()
{
  eve::resource::source source(this->path());
  load(*source);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void texture::setup(type_t type, const vec2u& size, unsigned depth, typeformat_t format, channels_t channels, filtermode_t filtermode, wrapmode_t wrapMode, bool mipmap)
{
  m_type = type;
  m_typeformat = format;
  m_channels = channels;
  m_filtermode = filtermode;
  m_wrapmode = wrapMode;
  m_mipmapped = mipmap;
  m_size.xy() = size;
  m_size.z = depth;

  if (!created())
    create_id();
}
