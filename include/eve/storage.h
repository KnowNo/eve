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

#include "debug.h"
#include "platform.h"
#include "allocator.h"
#include "detail/storage.h"
#include "uncopyable.h"

/** \addtogroup Lib
  * @{
  */

namespace eve
{

/** A POD type suitable for use as uninitialized storage for any object whose
  * size is at most Len and whose alignment requirement is a divisor of Align */
template<eve::size t_size, eve::size t_align = 8U>
class fixed_storage
{
public:
   /** @returns the size of fixed storage buffer. */
  eve::size size() const
  {
    return t_size;
  }

  /** Checks that @p newsize is at most t_size and always returns false
    * (no change occurred). */
  bool reserve(eve::size newsize)
  {
    eve_assert(newsize <= t_size);
    return false;
  }

  /** Statically asserts size and alignment of T match with this storage and
    * constructs T in this storage (calls its contructor) passing @p args to it. */
  template<class T, typename... Args>
  T* construct(Args&&... args)
  {
    return new (&m_data) T(std::forward<Args>(args)...);
  }

  /** \returns a non-const reference to the storage data reinterpreted as T. */
  template<class T>
  T& as() { return *reinterpret_cast<T*>(&m_data); }

  /** \returns a const reference to the storage data reinterpreted as T. */
  template<class T>
  const T& as() const { return *reinterpret_cast<const T*>(&m_data); }

  /** @returns a pointer to the buffer. */
  operator void*() {return &m_data; }

  /** @returns a pointer to the buffer. */
  operator const void*() const {return &m_data; }

private:
  detail::aligned_storage<t_size, t_align> m_data;
};

////////////////////////////////////////////////////////////////////////////////

template <eve::size t_size, eve::size t_align = 8U>
class dyn_storage
{
public:
  dyn_storage(const allocator::any& alloc = &eve::allocator::global())
    : m_allocator(alloc), m_size(k_size) { }

  ~dyn_storage()
  {
    if (exceeds())
      m_allocator.deallocate(dynptr());
  }

  /** @returns the size of current storage buffer. */
  eve::size size() const
  {
    return m_size;
  }

  /** @returns whether or not an dynamic allocation occurred to contain a size
    * request larger than fixed storage size.*/
  bool exceeds() const
  {
    return m_size > k_size;
  }

  /** Resizes, if possible, the storage buffer so that it becomes equal or
    * larger than @p size.
    * @returns true when the actual buffer was resized. */
  bool reserve(eve::size newsize)
  {
    if (newsize <= size())
      return false;

    newsize = (newsize / size() + 1) * k_size;

    if (exceeds())
      m_allocator.deallocate(dynptr());

    m_size = newsize | eve::size_msb;
    dynptr() = m_allocator.allocate(newsize, k_align);
    return true;
  }

  /** @returns a pointer to the buffer. */
  operator void*() 
  {
    if (exceeds())
      return dynptr();
    return (void*)m_storage;
  }

  /** @returns a pointer to the buffer. */
  operator const void*() const
  {
    if (exceeds())
      return dynptr();
    return (void*)m_storage;
  }

private:
  void*& dynptr() { return *(void**)&m_storage; }
  const void* dynptr() const { return *(const void* const*)&m_storage; }

  static const eve::size k_size = eve_max2(t_size, eve_sizeof(void*));
  static const eve::size k_align = eve_max2(t_align, eve_alignof(void*));

  allocator::any m_allocator;
  eve::size m_size;
  fixed_storage<k_size, k_align> m_storage;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

namespace storage {

/** TODO add doc */
template <class T, class Storage, typename... Args>
inline T* create(Storage& storage, Args&&... args)
{
  void* ptr;
  eve::size space = eve_sizeof(T);
  do
  {
    ptr = (void*)storage;
    ptr = eve::align(eve_alignof(T), eve_sizeof(T), ptr, space);
  } while (storage.reserve(space));
  return new (ptr) T(std::forward<Args>(args)...);
}

template <class T>
class unique_ptr : uncopyable
{
public:
  unique_ptr(T* object = nullptr)
    : m_object(object)
  {
  }

  ~unique_ptr()
  {
    reset();
  }
  
  unique_ptr(unique_ptr&& rhs)
    : m_object(rhs.m_object)
  {
    rhs.m_object;
  }

  void reset(T* other = nullptr)
  {
    if (m_object)
      m_object->~T();
    m_object = other;
  }

  T* release()
  {
    auto temp = m_object;
    m_object = nullptr;
    return temp;
  }

  operator T*() { return m_object; }
  operator const T*() const { return m_object; }
  T* operator->() { return m_object; }
  const T* operator->() const { return m_object; }

  unique_ptr& operator=(unique_ptr&& rhs)
  {
    reset(rhs.m_object);
    rhs.m_object = nullptr;
    return *this;
  }

private:
  T* m_object;
};

} // storage

} // eve

/** }@ */

