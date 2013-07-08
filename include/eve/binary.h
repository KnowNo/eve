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

#include "platform.h"
#include <string>

/** \addtogroup Lib
  * @{
  */

namespace eve {

class binarywriter
{
public:
  binarywriter(std::streambuf* buffer);
  ~binarywriter();

  /** Sets the buffer to write to. */
  void buffer(std::streambuf* buffer) { m_buffer = buffer; }

  /** Flushes the buffer. */
  void flush();

  binarywriter& operator<<(bool rhs) { write1(&rhs); return *this; }
  binarywriter& operator<<(eve::int8 rhs) { write1(&rhs); return *this; }
  binarywriter& operator<<(eve::uint8 rhs) { write1(&rhs); return *this; }
  binarywriter& operator<<(eve::int16 rhs) { write2(&rhs); return *this; }
  binarywriter& operator<<(eve::uint16 rhs) { write2(&rhs); return *this; }
  binarywriter& operator<<(eve::int32 rhs) { write4(&rhs); return *this; }
  binarywriter& operator<<(eve::uint32 rhs) { write4(&rhs); return *this; }
  binarywriter& operator<<(eve::int64 rhs) { write8(&rhs); return *this; }
  binarywriter& operator<<(eve::uint64 rhs) { write8(&rhs); return *this; }
  binarywriter& operator<<(float rhs) { write4(&rhs); return *this; }
  binarywriter& operator<<(double rhs) { write8(&rhs); return *this; }
  binarywriter& operator<<(const char* rhs);
  binarywriter& operator<<(const std::string& rhs);

private:
  void write1(const void* data);
  void write2(const void* data);
  void write4(const void* data);
  void write8(const void* data);

  std::streambuf* m_buffer;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class binaryreader
{
public:
  binaryreader(std::streambuf* buffer);

   /** Sets the buffer to read from. */
  void buffer(std::streambuf* buffer) { m_buffer = buffer; }

  binaryreader& operator>>(bool& rhs) { read1(&rhs); return *this; }
  binaryreader& operator>>(char& rhs) { read1(&rhs); return *this; }
  binaryreader& operator>>(unsigned char& rhs) { read1(&rhs); return *this; }
  binaryreader& operator>>(int16 & rhs) { read2(&rhs); return *this; }
  binaryreader& operator>>(uint16 & rhs) { read2(&rhs); return *this; }
  binaryreader& operator>>(int32 & rhs) { read4(&rhs); return *this; }
  binaryreader& operator>>(uint32 & rhs) { read4(&rhs); return *this; }
  binaryreader& operator>>(long long & rhs) { read8(&rhs); return *this; }
  binaryreader& operator>>(unsigned long long & rhs) { read8(&rhs); return *this; }
  binaryreader& operator>>(float& rhs) { read4(&rhs); return *this; }
  binaryreader& operator>>(double& rhs) { read8(&rhs); return *this; }
  binaryreader& operator>>(std::string& rhs);

private:
  void read1(void* data);
  void read2(void* data);
  void read4(void* data);
  void read8(void* data);

  std::streambuf* m_buffer;
};

} // eve

/** @} */
