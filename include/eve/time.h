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

#include <string>

/** \addtogroup Lib
  * @{
  */

namespace eve {

/** Provides a mechanism for measuring time. Will measure elapsed time since 
    last time it was reset. */
class stopwatch
{
public:
  /** @returns the duration in second of a tick on this machine. */
  static double tickduration();

  /** @returns a global timer that will measure time since application start. */
  static const stopwatch& global();
  
  /** @note will call reset(). */
  stopwatch();

  /** @returns the elapsed time since last reset and restarts time measurement. */
  double reset();

  /** @returns elapsed time in seconds since last reset(). */
  double elapsed() const;

  /** @returns ticks counted since last reset(). */
  long long ticks() const;

private:
  long long m_lasttick;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/** Provides a mechanism for measuring average Frames Per Second. */
class fpscounter
{
public:
  fpscounter();

  /** @returns the average number of FPS. */
  unsigned value() const { return m_fps; }

  /** Updates the counter.
      @note this function must be called once per frame. */
  bool tick();

private:
  stopwatch m_stopwatch;
  unsigned m_count;
  unsigned m_fps;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

namespace time {

/** Makes this thread sleep so that target fps @p target_fps is achieved.
    @param elapsed elapsed time since last frame.
    @param target_fps desired fps (e.g. 60). */
void fps_wait(float elapsed, unsigned target_fps);

} // time

} // eve

/** @} */
