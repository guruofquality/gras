// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

//Boost chrono has awesome high res timer!
//But its only in very recent boosts,
//so we have this little wrapper...

#ifndef INCLUDED_GRAS_CHRONO_HPP
#define INCLUDED_GRAS_CHRONO_HPP

#include <gras/gras.hpp>
#include <boost/version.hpp>

#if BOOST_VERSION >= 104700
#include <boost/chrono.hpp>

namespace gras
{

namespace chrono
{

typedef boost::chrono::time_point TimePoint;
typedef boost::chrono::nanoseconds Duration;

GRAS_FORCE_INLINE TimePoint high_res_now(void)
{
    return boost::posix_time::microsec_clock::universal_time();
}

GRAS_FORCE_INLINE long long duration_to_ticks(const Duration &d)
{
    return d.count();
}

GRAS_FORCE_INLINE unsigned long long tps(void)
{
    return (unsigned long long)(1e9);
}

} //namespace chrono

} //namespace gras

#else
#include <boost/date_time/posix_time/posix_time.hpp>

namespace gras
{

namespace chrono
{

typedef boost::posix_time::ptime TimePoint;
typedef boost::posix_time::time_duration Duration;

GRAS_FORCE_INLINE TimePoint high_res_now(void)
{
    return boost::posix_time::microsec_clock::universal_time();
}

GRAS_FORCE_INLINE long long duration_to_ticks(const Duration &d)
{
    return d.ticks();
}

GRAS_FORCE_INLINE unsigned long long tps(void)
{
    return boost::posix_time::time_duration::ticks_per_second();
}

} //namespace chrono

} //namespace gras

#endif

#endif /*INCLUDED_GRAS_CHRONO_HPP*/
