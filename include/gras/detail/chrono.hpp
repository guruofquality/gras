// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_DETAIL_CHRONO_HPP
#define INCLUDED_GRAS_DETAIL_CHRONO_HPP

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)

#include <windows.h>

namespace gras
{

    GRAS_FORCE_INLINE time_ticks_t time_now(void)
    {
        LARGE_INTEGER counts;
        QueryPerformanceCounter(&counts);
        return counts.QuadPart;
    }

    GRAS_FORCE_INLINE time_ticks_t time_tps(void)
    {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        return freq.QuadPart;
    }

} //namespace gras

#else

#include <ctime>

namespace gras
{

    GRAS_FORCE_INLINE time_ticks_t time_now(void)
    {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return ts.tv_sec*time_tps() + ts.tv_nsec;
    }

    GRAS_FORCE_INLINE time_ticks_t time_tps(void)
    {
        return 1000000000UL;
    }

} //namespace gras

#endif

namespace gras
{
    GRAS_FORCE_INLINE TimerAccumulate::TimerAccumulate(time_ticks_t &accum):
        accum(accum),
        start(time_now()),
        is_done(false)
    {
        //NOP
    }

    GRAS_FORCE_INLINE TimerAccumulate::~TimerAccumulate(void)
    {
        if (not is_done) this->done();
    }

    GRAS_FORCE_INLINE void TimerAccumulate::done(void)
    {
        accum += (time_now() - start);
        is_done = true;
    }
}

#endif /*INCLUDED_GRAS_DETAIL_CHRONO_HPP*/
