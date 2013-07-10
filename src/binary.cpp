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

#include "eve/binary.h"

using namespace eve;

inline uint16 swap2(uint16 val)
{
  return (val >> 8) | (val << 8);
}

inline uint32 swap4(uint32 val)
{
  return (val >> 24) | (val >> 8 & 0x0000FF00) | (val << 8 & 0x00FF0000) | (val << 24);
}

inline uint64 swap8(uint64 val)
{
  return (val >> 56) | (val >> 40 & 0x000000000000FF00) | (val >> 24 & 0x0000000000FF0000) | (val >> 8 & 0x00000000FF000000)
    | (val << 8 & 0x000000FF00000000) | (val << 24 & 0x0000FF0000000000) | (val << 40 & 0x00FF000000000000) | (val << 56);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

binarywriter::binarywriter(std::streambuf* buffer)
  : m_buffer(buffer) { }

binarywriter::~binarywriter()
{
  flush();
}

void binarywriter::flush()
{
  m_buffer->pubsync(); 
}

binarywriter& binarywriter::operator<<(const char* rhs)
{
  uint16 size = (uint16)strlen(rhs);
  *this << size;
  m_buffer->sputn(rhs, size);
  return *this;
}

binarywriter& binarywriter::operator<<(const std::string& rhs)
{
  uint16 size = (uint16)rhs.size();
  *this << size;
  m_buffer->sputn(rhs.c_str(), size);
  return *this;
}

void binarywriter::write1(const void* data)
{
  m_buffer->sputc(*reinterpret_cast<const char*> (data));
}

void binarywriter::write2(const void* data)
{
#ifdef EVE_BIG_ENDIAN
  uint16 t = swap2(*reinterpret_cast<const uint16*> (data));
  m_buffer->sputn(reinterpret_cast<const char*> (&t), 2);
#else
  m_buffer->sputn(reinterpret_cast<const char*> (data), 2);
#endif
}

void binarywriter::write4(const void* data)
{
#ifdef EVE_BIG_ENDIAN
  uint32 t = swap4(*reinterpret_cast<const uint32*> (data));
  m_buffer->sputn(reinterpret_cast<const char*> (&t), 4);
#else
  m_buffer->sputn(reinterpret_cast<const char*> (data), 4);
#endif
}

void binarywriter::write8(const void* data)
{
#ifdef EVE_BIG_ENDIAN
  uint64 t = swap8(*reinterpret_cast<const uint64*> (data));
  m_buffer->sputn(reinterpret_cast<const char*> (&t), 8);
#else
  m_buffer->sputn(reinterpret_cast<const char*> (data), 8);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

binaryreader::binaryreader(std::streambuf* buffer)
  : m_buffer(buffer) { }

binaryreader& binaryreader::operator>>(std::string& rhs)
{
  uint16 size;
  *this >> size;
  rhs.resize(size);
  m_buffer->sgetn(&rhs[0], size);
  rhs[size] = '\0';
  return *this;
}

void binaryreader::read1(void* data)
{
  *(char*)data = m_buffer->sbumpc();
}


void binaryreader::read2(void* data)
{
#ifdef EVE_BIG_ENDIAN
  m_buffer->sgetn(reinterpret_cast<char*> (data), 2);
  uint16* t = reinterpret_cast<uint16*> (data);
  *t = swap2(*t);
#else
  m_buffer->sgetn((char*) data, 2);
#endif
}

void binaryreader::read4(void* data)
{
#ifdef EVE_BIG_ENDIAN
  m_buffer->sgetn(reinterpret_cast<char*> (data), 4);
  uint32* t = reinterpret_cast<uint32*> (data);
  *t = swap4(*t);
#else
  m_buffer->sgetn((char*) data, 4);
#endif
}

void binaryreader::read8(void* data)
{
#ifdef EVE_BIG_ENDIAN
  m_buffer->sgetn(reinterpret_cast<char*> (data), 8);
  uint64* t = reinterpret_cast<uint64*> (data);
  *t = swap8(*t);
#else
  m_buffer->sgetn((char*)data, 8);
#endif
}