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

#include "eve/callstack.h"
#include "eve/debug.h"
#include <cstring>

using namespace eve;

#ifdef _MSC_VER

#include <Windows.h>
#include <DbgHelp.h>

static const HANDLE s_process = GetCurrentProcess();

callstack::symbol::symbol()
  : m_file("")
  , m_line(0)
{
  m_function[0] = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

callstack::callstack(bool capture, eve::size skipframes)
{
  m_hash = 0;
  m_size = 0;

  if (capture)
    this->capture(skipframes + 1);
}

void callstack::capture(eve::size skipframes)
{
  m_size = CaptureStackBackTrace(skipframes + 1, k_max_trace_size, m_trace, (PDWORD)&m_hash);
}


callstack::symbol callstack::fetch(eve::size index) const
{
  eve_assert(index < m_size);

  fixed_storage<sizeof(SYMBOL_INFO) + 64> storage;
  auto& symbolinfo = storage.as<SYMBOL_INFO>();
  symbolinfo.MaxNameLen = 64;
  symbolinfo.SizeOfStruct = sizeof(SYMBOL_INFO);
  symbolinfo.Name[0] = 0;
  symbolinfo.Address = 0;

  symbol symbol;

  if (SymFromAddr(s_process, (DWORD64)(m_trace[index]), 0, &symbolinfo))
  {
    memcpy(symbol.m_function, symbolinfo.Name, symbolinfo.NameLen);
    symbol.m_function[symbolinfo.NameLen] = 0;
  }
  
  IMAGEHLP_LINE info;
  info.SizeOfStruct = sizeof(IMAGEHLP_LINE);
  DWORD  dwDisplacement;
  if (SymGetLineFromAddr(s_process, (eve::uintptr)(symbolinfo.Address), &dwDisplacement, &info))
  {
    symbol.m_file = info.FileName;
    symbol.m_line = info.LineNumber + 1;
  }

  return symbol;
}

#else
#error Implement callstacks on this platform.
#endif

std::ostream& operator<<(std::ostream& output, const eve::callstack& callstack)
{
  for (eve::size i = 0; i < callstack.size(); ++i)
  {
    const auto& symbol = callstack.fetch(i);
    output << i + 1 << ") " << symbol.function() << "\n   In " << symbol.file() << ":" << symbol.line() << "\n";
  }
  return output;
}