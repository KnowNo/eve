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

#include "eve/time.h"
#include <thread>
#include <chrono>

#if defined(EVE_WINDOWS)
#  include <Windows.h>
#elif defined(EVE_LINUX)
#  error Implement timer on linux.
#endif

using namespace eve;

static long long s_frequency;
static double s_tickduration;

/** This dummy class gets the system frequency. */
static struct dummy
{
  dummy()
  {
    timeBeginPeriod(1);
    QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&s_frequency));
    s_tickduration = 1.0 / s_frequency;
  }
  ~dummy()
  {
    timeEndPeriod(1);
  }
} dummy_instance;

////////////////////////////////////////////////////////////////////////////////////////////////////

static stopwatch s_stopwatch;

double stopwatch::tickduration()
{
  return s_tickduration;
}

const stopwatch& stopwatch::global()
{
  return s_stopwatch;
}

stopwatch::stopwatch()
{
  reset();
}

double stopwatch::reset()
{
  auto ticks = this->ticks();
  auto elapsed = (ticks - m_lasttick) * s_tickduration;
  m_lasttick = ticks;
  return elapsed;
}

double stopwatch::elapsed() const
{
  auto ticks = this->ticks();
  return (ticks - m_lasttick) * s_tickduration;
}

long long stopwatch::ticks() const
{
  LARGE_INTEGER now;
  QueryPerformanceCounter(&now);
  return now.QuadPart;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

fpscounter::fpscounter()
  : m_count(0), m_fps(0)
{
}

bool fpscounter::tick()
{
  ++m_count;
  if (m_stopwatch.elapsed() > 1.0f)
  {
    m_fps = unsigned(float(m_count) / float(m_stopwatch.elapsed()));
    m_count = 0;
    m_stopwatch.reset();
    return true;
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void time::fps_wait(float elapsed, unsigned target_fps)
{
  const float kfrequency = 1.0f / target_fps;
  if (elapsed < kfrequency)
    std::this_thread::sleep_for(
      std::chrono::milliseconds(unsigned((kfrequency - elapsed) * 1000))
    );
}
