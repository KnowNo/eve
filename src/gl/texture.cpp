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

#define GLEW_STATIC
#include "gl/glew.h"

using namespace eve;

static const GLint k_gl_type[] = { GL_TEXTURE_2D, GL_TEXTURE_3D, GL_TEXTURE_RECTANGLE, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BUFFER };
static const GLint k_gl_internal_format[][4] = {
  /* UNSIGNED_BYTE */ {GL_R8, GL_RG8, GL_RGB8, GL_RGBA8},
  /* half_float */    {GL_R16F, GL_RG16F, GL_RGB16F, GL_RGBA16F},
  /* FLOAT */         {GL_R32F, GL_RG32F, GL_RGB16F, GL_RGBA16F}
};
static const GLenum k_gl_pixelformat[] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };
static const GLenum k_gl_typeformat[] = { GL_UNSIGNED_BYTE, GL_FLOAT, GL_FLOAT };
static const GLint k_gl_wrapmode[] = { GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER };

void texture::unbind(type_t type)
{
  glBindTexture(k_gl_type[(unsigned)type], 0);
}

void texture::bind(unsigned unit) const
{
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(k_gl_type[static_cast<unsigned>(m_type)], m_pimpl.as<GLuint>());
}

void texture::destroy()
{
  glDeleteTextures(1, &m_pimpl.as<GLuint>());
  m_pimpl.as<GLuint>() = 0;
}

void texture::init_id()
{
  m_pimpl.as<GLuint>() = 0;
}

bool texture::created() const
{ 
  return m_pimpl.as<GLuint>() != 0; 
}

void texture::create_id()
{
  glGenTextures(1, &m_pimpl.as<GLuint>());
}

void texture::write(const void* data)
{
  GLenum typeFormat = k_gl_typeformat[static_cast<unsigned>(m_typeformat)];
  if(!data && (m_typeformat == typeformat::half_float))
    typeFormat = GL_FLOAT;
  switch (m_type)
  {
  case type::tex2D:
  case type::rectangle:
    glTexImage2D(
      k_gl_type[int(m_type)], // target
      0,             // level
      k_gl_internal_format[int(m_typeformat)][int(m_channels)], // internalFormat
      width(), height(), 0, // width, height, border
      k_gl_pixelformat[static_cast<unsigned>(m_channels)],
      typeFormat,
      data);
    break;

  case type::tex3D:
  case type::array2D:
    glTexImage3D(
      k_gl_type[int(m_type)], // target
      0,             // level
      k_gl_internal_format[int(m_typeformat)][int(m_channels)], // internalFormat
      width(), height(), depth(), 0, // width, height, depth, border
      k_gl_pixelformat[static_cast<unsigned>(m_channels)],
      typeFormat,
      data);
    break;

  default:
    eve_internal_error;
  }

  if (m_mipmapped)
    glGenerateMipmap(k_gl_type[int(m_type)]);
}

void texture::subwrite(unsigned depth, const void* data)
{
  GLenum typeFormat = k_gl_typeformat[static_cast<unsigned>(m_typeformat)];
  if(!data && (m_typeformat == texture::typeformat::half_float))
    typeFormat = GL_FLOAT;
  switch (m_type)
  {
  case type::tex2D:
  case type::rectangle:
    glTexSubImage2D(
      k_gl_type[int(m_type)], // target
      0, 0, 0, // level, xoffset, yoffset
      width(), height(), // width, height
      k_gl_pixelformat[static_cast<unsigned>(m_channels)],
      typeFormat,
      data);
    break;

  case type::tex3D:
  case type::array2D:
    glTexSubImage3D(
      k_gl_type[int(m_type)], // target
      0, 0, 0, // level, xoffset, yoffset
      depth,
      width(), height(), 1, // width, height, depth
      k_gl_pixelformat[static_cast<unsigned>(m_channels)],
      typeFormat,
      data);
    break;

  default:
    eve_internal_error;
  }

  if (m_mipmapped)
    glGenerateMipmap(k_gl_type[int(m_type)]);
}

void texture::set_filters()
{
  GLint minFilterID = 0;
  GLint magFilterID =  m_filtermode == filtermode::nearest ? GL_NEAREST : GL_LINEAR;
  if (m_mipmapped)
  {
    if (m_filtermode == filtermode::nearest)
      minFilterID = GL_NEAREST_MIPMAP_NEAREST;
    else
      minFilterID = GL_LINEAR_MIPMAP_LINEAR;
  } else
    if (m_filtermode == filtermode::nearest)
      minFilterID = GL_NEAREST;
    else
      minFilterID = GL_LINEAR;

  GLint textureType = k_gl_type[static_cast<unsigned>(m_type)];
  glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, minFilterID);
  glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, magFilterID);

#ifdef GLEW_EXT_texture_filter_anisotropic
  //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, mpDevice->getMaxAnisotropic());
#endif
}

void texture::set_clampmode()
{
  GLint textureType = k_gl_type[static_cast<unsigned>(m_type)];
  GLint wrapMode = k_gl_wrapmode[static_cast<unsigned>(m_wrapmode)];
  glTexParameteri(textureType, GL_TEXTURE_WRAP_S, wrapMode);
  glTexParameteri(textureType, GL_TEXTURE_WRAP_T, wrapMode);
}

//void texture::setBorderColor(const Colorf& color) const
//{
//  GLint textureType = k_gl_type[static_cast<unsigned>(m_type)];
//  glBindTexture(textureType, 0);
//  glTexParameterfv(k_gl_type[static_cast<unsigned>(m_type)], GL_TEXTURE_BORDER_COLOR, color);
//  glBindTexture(textureType, 0);
//}


//void texture::setBuffer(unsigned id)
//{
//  activate();
//  glTexBuffer(GL_TEXTURE_BUFFER, k_gl_internal_format[int(mTypeFormat)][int(mChannels)], id);
//  unbind(m_type);
//}
