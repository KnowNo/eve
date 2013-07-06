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

#include "eve/platform.h"

namespace eve { namespace allocator {

/** This class is used to interface any allocator by type-erasure. 
    @note it does not own the allocator therefore it is not deleted by this class
          on destruction. */
class any
{
public:
  template <class tallocator>
  any(tallocator* allocator)
    : m_allocator(allocator)
  {
    static const calltable s_table = { any::allocate<tallocator>, any::deallocate<tallocator> };
    m_table = &s_table;
  }

  void* allocate(eve::size size, uint8 align)
  {
    return m_table->allocate(m_allocator, size, align);
  }

  void  deallocate(const void* ptr)
  {
    m_table->deallocate(m_allocator, ptr);
  }
  
private:
  struct calltable
  {
    void* (*allocate)(void*, eve::size, uint8);
    void (*deallocate)(void*, const void*);
  };

  template<class T>
  static void* allocate(void* alloc, eve::size size, uint8 align)
  {
    return static_cast<T*>(alloc)->allocate(size, align);
  }

  template<class T>
  static void deallocate(void* alloc, const void* ptr)
  {
    static_cast<T*>(alloc)->deallocate(ptr);
  }

  void* m_allocator;
  const calltable* m_table;
};

} // allocator
} // eve
