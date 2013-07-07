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
#include "debug.h"
#include "platform.h"
#include "uncopyable.h"
#include <memory>

/** \addtogroup Lib
  * @{
  */

namespace eve {

/** Allocates memory using specified allocator on construction and deallocates it on destruction.
    This class behaves as a unique resource handler (e.g. std::unique_ptr) */
template <class Allocator>
class unique_alloc : uncopyable
{
public:
  unique_alloc(eve_source_location_args, Allocator& alloc, eve::size size, eve::size align)
    : m_allocator(alloc)
  {
    m_ptr = alloc.allocate(eve_forward_source_location_args, size, align);
  }
  ~unique_alloc()
  {
    if (m_ptr)
      m_allocator.deallocate(eve_here, m_ptr);
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

template <typename T, typename Allocator, typename... Args>
T* create(eve_source_location_args, Allocator& allocator, Args&&... args)
{
  eve::unique_alloc<Allocator> alloc(eve_forward_source_location_args, allocator, eve_sizeof(T), eve_alignof(T));
  auto object = new (alloc) T(std::forward<Args>(args)...);
  alloc.release();
  return object;
}

/** In-place create object T using @p args in preallocated memory pointed by @p ptr. */
template <typename T, typename... Args>
T* create(eve_source_location_args, void* ptr, Args&&... args)
{
  new (ptr) T(std::forward<Args>(args)...);
  eve::memory_debugger::track(eve_forward_source_location_args, ptr, true);
  return static_cast<T*>(ptr);
}

template <typename T, typename Allocator>
void destroy(eve_source_location_args, Allocator& allocator, const T* object)
{
  object->~T();
  allocator::global().deallocate(eve_forward_source_location_args, object);
}

/** Destroyes @p object in place (without deallocating any memory). 
    @note to be valid, object must have been created in-place. */
template <typename T>
void destroy(eve_source_location_args, const T* object)
{
  eve::memory_debugger::untrack(eve_forward_source_location_args, object, true);
  object->~T();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
struct global_deleter
{
  void operator()(T* ptr) const
  {
    destroy(eve_here, allocator::global(), ptr);
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO move this to some other header file
template<class T>
struct unique_ptr
{
  // This is because damn Visual Studio does not support C++11 aliases.
  typedef std::unique_ptr<T, global_deleter<T>> type;
};

template<class T, typename... Args>
typename unique_ptr<T>::type make_unique(Args&&... args)
{
  return typename unique_ptr<T>::type(new T(std::forward<Args>(args)...));
}

} // eve
