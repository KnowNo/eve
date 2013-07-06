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

#include "eve/pixelbuffer.h"
#include "eve/exceptions.h"

#include <png.h>

#define PNGSIGSIZE 8

using namespace eve;

pixelbuffer::pixelbuffer()
  : m_data(nullptr)
  , m_ratio(0)
  , m_channels(0)
  , m_bits_per_channel(0)
  , m_bits_per_pixel(0)
{ }

pixelbuffer::~pixelbuffer()
{
  unload();
}

void pixelbuffer::load(std::istream& source)
{
  if (validatePNG(source))
    loadPNG(source);
  throw eve::system_error("Image is not a valid PNG.");
}

void pixelbuffer::unload()
{
  delete[] m_data;
  m_data = nullptr;
}

void pixelbuffer::on_reload()
{
  resource::source source(path());
  unload();
  load(*source);
}

bool pixelbuffer::validatePNG(std::istream& source)
{
  //Allocate a buffer of 8 bytes, where we can put the file signature.
  png_byte pngsig[PNGSIGSIZE];
  int is_png = 0;

  //Read the 8 bytes from the stream into the sig buffer.
  source.read((char*)pngsig, PNGSIGSIZE);

  //Check if the read worked...
  if (!source.good()) return false;

  //Let LibPNG check the sig. If this function returns 0, everything is OK.
  is_png = png_sig_cmp(pngsig, 0, PNGSIGSIZE);
  return (is_png == 0);
}

void userReadData(png_structp pngPtr, png_bytep data, png_size_t length)
{
  // Read "length" bytes from the stream in pngPtr to "data".
  png_voidp a = png_get_io_ptr(pngPtr);
  ((std::istream*)a)->read((char*)data, length);
}

bool pixelbuffer::loadPNG(std::istream& source)
{
  //Here we create the png read struct. The 3 NULL's at the end can be used
  //for your own custom error handling functions, but we'll just use the default.
  //if the function fails, NULL is returned.
  png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!pngPtr)
    throw eve::system_error("Couldn't initialize png read struct");

  //Here we create the png info struct.
  //Note that this time, if this function fails, we have to clean up the read struct!
  png_infop infoPtr = png_create_info_struct(pngPtr);
  if (!infoPtr)
  {
    png_destroy_read_struct(&pngPtr, (png_infopp)0, (png_infopp)0);
    throw eve::system_error("Couldn't initialize png info struct.");
  }

  // Needed later, defined here for error handling.
  png_bytep* rowPtrs = nullptr;

  // Error handling block.
  if (setjmp(png_jmpbuf(pngPtr)))
  {
    //An error occured, so clean up what we have allocated so far...
    png_destroy_read_struct(&pngPtr, &infoPtr,(png_infopp)0);
    delete[] rowPtrs;
    delete[] m_data;
    m_data = nullptr;
    throw eve::system_error("An error occured while reading the PNG file.");
  }

  // Set reading function so we can read from std::istream
  png_set_read_fn(pngPtr,(png_voidp)&source, userReadData);

  //Set the amount signature bytes we've already read:
  //We've defined PNGSIGSIZE as 8;
  png_set_sig_bytes(pngPtr, PNGSIGSIZE);

  //Now call png_read_info with our pngPtr as image handle, and infoPtr to receive the file info.
  png_read_info(pngPtr, infoPtr);

  m_size.x = png_get_image_width(pngPtr, infoPtr);
  m_size.y = png_get_image_height(pngPtr, infoPtr);

  //bits per CHANNEL! note: not per pixel!
  m_bits_per_channel = png_get_bit_depth(pngPtr, infoPtr);
  //Number of channels
  m_channels = png_get_channels(pngPtr, infoPtr);
  //Color type. (RGB, RGBA, Luminance, luminance alpha... palette... etc)
  png_uint_32 color_type = png_get_color_type(pngPtr, infoPtr);

  switch (color_type) {
  case PNG_COLOR_TYPE_PALETTE:
    png_set_palette_to_rgb(pngPtr);
    //Don't forget to update the channel info (thanks Tom!)
    //It's used later to know how big a buffer we need for the image
    m_channels = 3;
    break;

  case PNG_COLOR_TYPE_GRAY:
    if (m_bits_per_channel < 8)
      png_set_expand_gray_1_2_4_to_8(pngPtr);
    //And the bitdepth info
    m_bits_per_channel = 8;
    m_channels = 1;
    break;
  }

  /*if the image has a transperancy set.. convert it to a full Alpha channel..*/
  if (png_get_valid(pngPtr, infoPtr, PNG_INFO_tRNS))
  {
    png_set_tRNS_to_alpha(pngPtr);
    m_channels += 1;
  }

  m_bits_per_pixel = m_bits_per_channel * m_channels;

  //We don't support 16 bit precision.. so if the image Has 16 bits per channel
  //precision... round it down to 8.
  if (m_bits_per_channel == 16)
    png_set_strip_16(pngPtr);

  //Here's one of the pointers we've defined in the error handler section:
  //Array of row pointers. One for every row.
  rowPtrs = new png_bytep[m_size.y];

  //Allocate a buffer with enough space.
  //This pointer was also defined in the error handling section, so we can clean it up on error.
  m_data = new char[m_size.x * m_size.y * m_bits_per_pixel / 8];
  //This is the length in bytes, of one row.
  const unsigned int stride = m_size.x * m_bits_per_pixel / 8;

  //A little for-loop here to set all the row pointers to the starting
  //adresses for every row in the buffer
  for (unsigned i = 0; i < m_size.y; i++)
  {
    //Set the pointer to the data pointer + i times the row stride.
    //Notice that the row order is reversed with q.
    //This is how at least OpenGL expects it,
    //and how many other image loaders present the data.
    png_uint_32 q = (m_size.y - i - 1) * stride;
    rowPtrs[i] = (png_bytep)m_data + q;
  }

  //And here it is! The actual reading of the image!
  //Read the imagedata and write it to the addresses pointed to
  //by rowptrs (in other words: our image databuffer)
  png_read_image(pngPtr, rowPtrs);

  delete[] rowPtrs;

  return true;
}
