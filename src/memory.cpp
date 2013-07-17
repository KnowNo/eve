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

#include "eve/memory.h"

void* operator new(size_t size, eve::allocator::any allocator)
{
  if (size == 0)
    size = 1;
  return allocator.allocate(eve::size(size), 8U);
}

void operator delete(void* ptr, eve::allocator::any allocator)
{
  allocator.deallocate(ptr);
}


void* operator new(size_t size, eve::detail::place place) throw()
{
  eve::memory_debugger::track(place.address, true);
  return place.address;
}

void operator delete(void* ptr, eve::detail::place place) throw()
{
  eve::memory_debugger::untrack(place.address, true);
}


void* operator new[](size_t size, eve::allocator::any allocator)
{
  if (size == 0)
    size = 1;
  return allocator.allocate(eve::size(size), 8U);
}

void operator delete[](void* ptr, eve::allocator::any allocator)
{
  allocator.deallocate(ptr);
}
