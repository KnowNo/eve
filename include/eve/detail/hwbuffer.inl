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

namespace eve {
namespace hwarray {

template <typename VertexType>
vertices<VertexType>::vertices()
{
}

template <typename VertexType>
vertices<VertexType>::vertices(hwbuffer* buffer, bool instanced)
{
  setup(buffer, instanced);
}

template <typename VertexType>
vertices<VertexType>::vertices(hwbuffer* buffer, eve::size offset, eve::size capacity, bool instanced)
{
  setup(buffer, offset, capacity, instanced);
}


template <typename VertexType>
void vertices<VertexType>::setup(hwbuffer* buffer, bool instanced)
{
  base::setup(buffer, eve_sizeof(VertexType), instanced);
}

template <typename VertexType>
void vertices<VertexType>::setup(hwbuffer* buffer, eve::size offset, eve::size capacity, bool instanced)
{
  base::setup(buffer, eve_sizeof(VertexType), offset, capacity, instanced);
}

template <typename VertexType>
void write(const VertexType* data, const eve::size size)
{
  base::write(eve_sizeof(VertexType), 0, data, size);
}

template <typename VertexType>
void write(eve::size first, const VertexType* data, const eve::size size)
{
  base::write(eve_sizeof(VertexType), first, data, size);
}

} // eve
} // hwarray