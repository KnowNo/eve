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
#include "uncopyable.h"
#include <memory>

/** \addtogroup Lib
  * @{
  */

namespace eve {

/** If it is possible to fit size bytes of storage aligned by alignment into the
 ** buffer pointed to by @p ptr with length @p space, the function sets @p space
 ** to the total size of object plus bytes used for alignment.
 ** @returns the first possible address of such aligned storage. If it is
 ** impossible (i.e. the buffer is too small), align does nothing and returns
 ** nullptr. */
inline  void* align(eve::size align, eve::size size, void* ptr, eve::size& space)
{
  eve::size offset = (eve::size)((eve::uintptr)ptr & (align - 1));
  
  if (offset > 0)
    offset = align - offset;

  if (!ptr || space < offset || space - offset < size)
    return nullptr;
  else
  {
    ptr = (char*)ptr + offset;
    space = offset + size;
    return ptr;
  }
}

////////////////////////////////////////////////////////////////////////////////

namespace allocator
{

class  any
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

////////////////////////////////////////////////////////////////////////////////

class  heap
{
public:
  void* allocate(eve::size size, uint8 align);
  void  deallocate(const void* ptr);
};

////////////////////////////////////////////////////////////////////////////////

 heap& global();

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Allocator>
class scoped_alloc : uncopyable
{
public:
  scoped_alloc(Allocator& alloc, eve::size size, eve::size align)
    : m_allocator(alloc)
  {
    m_ptr = alloc.allocate(size, align);
  }
  ~scoped_alloc()
  {
    if (m_ptr)
      m_allocator.deallocate(m_ptr);
  }
  void* release()
  {
    auto temp = m_ptr;
    m_ptr = nullptr;
    return temp;
  }
  operator void*() { return m_ptr;}
private:
  Allocator& m_allocator;
  void* m_ptr;
};

}// allocator

template <class T, class Allocator, typename... Args>
T* create(Allocator& allocator, Args&&... args)
{
  allocator::scoped_alloc<Allocator> alloc(allocator, eve_sizeof(T), eve_alignof(T));
  new (alloc) T(std::forward<Args>(args)...);
  return static_cast<T*>(alloc.release());
}

template <class T>
void destroy(const T* object)
{
  object->~T();
}

template <class T, class Allocator>
void destroy(Allocator& allocator, const T* object)
{
  object->~T();
  allocator::global().deallocate(object);
}

template <class T>
struct global_deleter
{
  void operator()(T* ptr) const
  {
    destroy(allocator::global(), ptr);
  }
};

template<class T>
struct unique_ptr
{
  // This is because damn Visual Studio does not support template aliases.
  typedef std::unique_ptr<T, global_deleter<T>> type;
};

template<class T, typename... Args>
typename unique_ptr<T>::type make_unique(Args&&... args)
{
  return typename unique_ptr<T>::type(new T(std::forward<Args>(args)...));
}

} // eve

/** }@ */
