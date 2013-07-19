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
#include "resource.h"

/**
 * \addtogroup Graphics
 * @{
 */
  
namespace eve
{

class pixelbuffer : public resource
{
public:
  pixelbuffer();
  ~pixelbuffer();
 
  const char* data() const { return m_data; }
  const vec2u& size() const { return m_size; }
  eve::size width() const { return m_size.x; }
  eve::size height() const { return m_size.y; }
  float ratio() const { return m_ratio; }
  uint16 channels() const { return m_channels; }
  uint16 bits_per_channel() const { return m_bits_per_channel; }
  uint16 bits_per_pixel() const { return m_bits_per_pixel; }

  void load(std::istream& source) override;
  void unload();

private:
  void on_reload() override;
  bool validatePNG(std::istream& source);
  bool loadPNG(std::istream& source);

  char* m_data;
  vec2u m_size;
  float m_ratio;
  uint16 m_channels;
  uint16 m_bits_per_channel;
  uint16 m_bits_per_pixel;
};

} //eve

/** }@ */
