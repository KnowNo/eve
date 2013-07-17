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

#include "allocator.h"
#include "allocators/any.h"
#include "debug.h"
#include "platform.h"
#include "uncopyable.h"
#include <memory>

/** \addtogroup Lib
  * @{
  */

#define eve_new new(eve::allocator::any(&eve::allocator::global()))
#define eve_inplace_new(ptr) new(eve::detail::place(ptr))

namespace eve {

/** Allocates memory using specified allocator on construction and deallocates it on destruction.
    This class behaves as a unique resource handler (e.g. std::unique_ptr) */
template <class Allocator>
class unique_alloc : uncopyable
{
public:
  unique_alloc(Allocator& alloc, eve::size size, eve::size align)
    : m_allocator(alloc)
  {
    m_ptr = alloc.allocate(size, align);
  }
  ~unique_alloc()
  {
    if (m_ptr)
      m_allocator.deallocate(m_ptr);
  }
  void* release() throw()
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

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T, typename Allocator>
void destroy(Allocator& allocator, const T* object)
{
  object->~T();
  allocator.deallocate(object);
}

template <typename T>
void global_destroy(const T* object)
{
  destroy(eve::allocator::global(), object);
} 

/** Destroyes @p object in place (without deallocating any memory). 
    @note to be valid, object must have been created in-place. */
template <typename T>
void destroy(const T* object)
{
  eve::memory_debugger::untrack(object, true);
  object->~T();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T, typename Allocator>
void destroy_array(Allocator& allocator, const T* array)
{
  if (std::has_trivial_destructor<T>::value)
  {
    allocator.deallocate(array);
  } else
  {
    auto ptr = reinterpret_cast<const uint32*>(array) - 1;
    for (uint32 i = 0; i < *ptr; ++i)
      array[i].~T();
    allocator.deallocate(ptr);
  }
}

template <typename T>
void global_destroy_array(const T* array)
{
  destroy_array(eve::allocator::global(), array);
} 

/** Destroyes @p object in place (without deallocating any memory). 
    @note to be valid, object must have been created in-place. */
template <typename T>
void destroy_array(const T* array)
{
  if (std::has_trivial_destructor<T>::value)
  {
    eve::memory_debugger::untrack(array, true);
  } else
  {
    auto ptr = reinterpret_cast<const uint32*>(array) - 1;
    for (uint32 i = 0; i < *ptr; ++i)
      array[i].~T();
    eve::memory_debugger::untrack(ptr, true);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
struct global_deleter
{
  void operator()(T* ptr) const
  {
    destroy(allocator::global(), ptr);
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct unique_ptr
{
  typedef std::unique_ptr<T, global_deleter<T>> type;
};

template<class T, typename... Args>
typename unique_ptr<T>::type make_unique(Args&&... args)
{
  return typename unique_ptr<T>::type(eve_new T(std::forward<Args>(args)...));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO move this into detail/memory.h
namespace detail {

  struct place
  {
    void* address;
    explicit place(void* address) : address(address) { }
  };

}

} // eve

void* operator new(size_t size, eve::allocator::any allocator);
void operator delete(void* ptr, eve::allocator::any allocator);

void* operator new(size_t size, eve::detail::place place);
void operator delete(void* ptr, eve::detail::place place);

void* operator new[](size_t size, eve::allocator::any allocator);
void operator delete[](void* ptr, eve::allocator::any allocator);
