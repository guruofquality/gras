// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

//Boost chrono has awesome high res timer!
//But its only in very recent boosts,
//so we have this little wrapper...
//now/tps inspired by libnumanuma.

#ifndef INCLUDED_GRAS_CHRONO_HPP
#define INCLUDED_GRAS_CHRONO_HPP

#include <gras/gras.hpp>

namespace gras
{
    typedef long long time_ticks_t;

    //! Get the time now in tick counts
    time_ticks_t time_now(void);

    //! Get the number of ticks per second
    time_ticks_t time_tps(void);

    /*!
     * Timer Accumulate creates a scoped timer object,
     * which adds the time elapsed into the accumulator.
     * Typically, the time elapsed is the absolute
     * destructor time - absolute constructor time.
     * However, the user may call done() to accumulate
     * the time elapsed before the destructor is called.
     */
    struct TimerAccumulate
    {
        //! Create a new timer object given the accumulator
        TimerAccumulate(time_ticks_t &accum);

        //! Destructor accumulates the elapsed time
        ~TimerAccumulate(void);

        //! Accumulate elapsed time before destructor
        void done(void);

        time_ticks_t &accum;
        time_ticks_t start;
        bool is_done;
    };

}

//--------------------------------------------------------------------//
//------------------ implementation details below --------------------//
//--------------------------------------------------------------------//

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
        return ts.tv_sec*1000000000UL + ts.tv_nsec;
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

#endif /*INCLUDED_GRAS_CHRONO_HPP*/
