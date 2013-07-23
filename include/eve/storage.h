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

#include "detail/storage.h"
#include "memory.h"
#include "debug.h"
#include "platform.h"
#include "allocators/any.h"
#include "uncopyable.h"

/** \addtogroup Lib
  * @{
  */

namespace eve
{

/** A POD type suitable for use as uninitialized storage for any object whose
  * size is at most Len and whose alignment requirement is a divisor of Align */
template<eve::size Size, eve::size Align = 8U>
class fixed_storage : private uncopyable
{
public:
   /** @returns the size of fixed storage buffer. */
  eve::size size() const
  {
    return Size;
  }

  /** Checks that @p newsize is at most Size and always returns false
    * (no change occurred). */
  bool reserve(eve::size newsize)
  {
    eve_assert(newsize <= Size);
    return false;
  }

  /** Statically asserts size and alignment of T match with this storage and
    * constructs T in this storage (calls its contructor) passing @p args to it. */
  template<class T, typename... Args>
  T* construct(Args&&... args)
  {
    static_assert(eve_sizeof(T) <= Size, "Storage size must be at least equal to those of type T.");
    static_assert(eve_alignof(T) <= Align, "Storage align must be at least equal to those of type T.");
    auto ptr = new (&m_data) T(std::forward<Args>(args)...);
    eve::memory_debugger::track(ptr, true);
    return ptr;
  }

  /** \returns a non-const reference to the storage data reinterpreted as T. */
  template<class T>
  T& as() { return *reinterpret_cast<T*>(&m_data); }

  /** \returns a const reference to the storage data reinterpreted as T. */
  template<class T>
  const T& as() const { return *reinterpret_cast<const T*>(&m_data); }

  /** @returns a pointer within this storage with specified alignment @p align point to
      a block of memory of a least @p size large. */
  void* align(eve::size size, eve::size align = 8U)
  {
    return const_cast<void*>(static_cast<const fixed_storage*>(this)->align(size, align));
  }

  /** @returns a pointer within this storage with specified alignment @p align point to
      a block of memory of a least @p size large.
      @note if not enough room, assertion is called. */
  const void* align(eve::size size, eve::size align = 8U) const
  {
    eve::size space = size;
    const void* ptr = eve::align(align, size, (const void*)(*this), space);
    eve_assert(space <= Size);
    return ptr;
  }

  /** @returns a pointer to the buffer. */
  operator void*() {return &m_data; }

  /** @returns a pointer to the buffer. */
  operator const void*() const {return &m_data; }

private:
  detail::aligned_storage<Size, Align> m_data;
};

////////////////////////////////////////////////////////////////////////////////

template <eve::size Size, eve::size Align = 8U>
class dyn_storage : private uncopyable
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

  /** @returns a pointer within this storage with specified alignment @p align point to
      a block of memory of a least @p size large.
      @note this function may need to call reserve() to make more room. */
  void* align(eve::size size, eve::size align = 8U)
  {
    void* ptr;
    eve::size space = size;
    do
    {
      ptr = (void*)*this;
      ptr = eve::align(align, size, ptr, space);
    } while (reserve(space));
    return ptr;
  }

  /** @returns a pointer within this storage with specified alignment @p align point to
      a block of memory of a least @p size large.
      @note if not enough room, assertion is called. */
  const void* align(eve::size size, eve::size align = 8U) const
  {
    eve::size space = size;
    const void* ptr = eve::align(align, size, (const void*)(*this), space);
    eve_assert(space <= m_size);
    return ptr;
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

  static const eve::size k_size = eve_max2(Size, eve_sizeof(void*));
  static const eve::size k_align = eve_max2(Align, eve_alignof(void*));

  allocator::any m_allocator;
  eve::size m_size;
  fixed_storage<k_size, k_align> m_storage;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

namespace storage {

/** @returns a T pointer inside of @p storage with correct T alignment. */
template <class T, class Storage>
const T* align(const Storage& storage)
{
  return storage.align(eve_sizeof(T), eve_alignof(T));
}

/** @returns a T pointer inside of @p storage with correct T alignment.
    @note if current storage space is not enough, reserve() will be called to make enough room. */
template <class T, class Storage>
T* align(Storage& storage)
{
  return storage.align(eve_sizeof(T), eve_alignof(T));
}

} // storage

} // eve

template <eve::size Size, eve::size Align>
void* operator new(size_t size, eve::fixed_storage<Size, Align>& storage)
{
  auto ptr = storage.align(eve::size(size), 8U);
  eve::memory_debugger::track(ptr, true);
  return ptr;
}

template <eve::size Size, eve::size Align>
void operator delete(void* ptr, eve::fixed_storage<Size, Align>& storage)
{
  eve::memory_debugger::untrack(ptr, true);
}


template <eve::size Size, eve::size Align>
void* operator new(size_t size, eve::dyn_storage<Size, Align>& storage)
{
  auto ptr = storage.align(eve::size(size), 8U);
  eve::memory_debugger::track(ptr, true);
  return ptr;
}

template <eve::size Size, eve::size Align>
void operator delete(void* ptr, eve::dyn_storage<Size, Align>& storage)
{
  eve::memory_debugger::untrack(ptr, true);
}

/** }@ */

