// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/time_tag.hpp>
#include <PMC/Containers.hpp>
#include <boost/cstdint.hpp> //uint64
#include <boost/math/special_functions/round.hpp>

#define TIME_TAG_TPS time_ticks_t(1000000000UL)

using namespace gras;

static TimeTag &normalize(TimeTag &t)
{
    int num = int(t._ticks/TIME_TAG_TPS);
    if (num < 0) num--; //stops negative ticks
    t._fsecs += num;
    t._ticks -= num*TIME_TAG_TPS;
    return t;
}

TimeTag::TimeTag(void):
    _fsecs(0), _ticks(0)
{/*NOP*/}

TimeTag TimeTag::from_ticks(const time_ticks_t ticks)
{
    TimeTag t;
    t._ticks = ticks;
    return normalize(t);
}

TimeTag TimeTag::from_ticks(const time_ticks_t ticks, const double rate)
{
    TimeTag t;
    t._fsecs = time_ticks_t(ticks/rate);
    const double error = ticks - (t._fsecs*rate);
    t._ticks = boost::math::llround((error*TIME_TAG_TPS)/rate);
    return normalize(t);
}

TimeTag TimeTag::from_pmc(const PMCC &p)
{
    TimeTag t;
    const PMCTuple<2> &tuple = p.as<PMCTuple<2> >();
    t._fsecs = tuple[0].as<boost::uint64_t>();
    t._ticks = boost::math::llround(tuple[1].as<double>()*TIME_TAG_TPS);
    return normalize(t);
}

time_ticks_t TimeTag::to_ticks(void) const
{
    return _fsecs*TIME_TAG_TPS + _ticks;
}

time_ticks_t TimeTag::to_ticks(const double rate) const
{
    const time_ticks_t full = time_ticks_t(_fsecs*rate);
    const double error = _fsecs - (full/rate);
    return full + boost::math::llround(_ticks*rate/TIME_TAG_TPS + error*rate);
}

PMCC TimeTag::to_pmc(void) const
{
    PMCTuple<2> tuple;
    tuple[0] = PMC_M<boost::uint64_t>(_fsecs);
    tuple[1] = PMC_M<double>(_ticks/double(TIME_TAG_TPS));
    return PMC_M(tuple);
}

TimeTag &TimeTag::operator+=(const TimeTag &rhs)
{
    _fsecs += rhs._fsecs;
    _ticks += rhs._ticks;
    return normalize(*this);
}

TimeTag &TimeTag::operator-=(const TimeTag &rhs)
{
    _fsecs -= rhs._fsecs;
    _ticks -= rhs._ticks;
    return normalize(*this);
}

bool gras::operator<(const TimeTag &lhs, const TimeTag &rhs)
{
    if (lhs._fsecs == rhs._fsecs) return lhs._ticks < rhs._ticks;
    return lhs._fsecs < rhs._fsecs;
}

bool gras::operator==(const TimeTag &lhs, const TimeTag &rhs)
{
    return (lhs._fsecs == rhs._fsecs) and (lhs._ticks == rhs._ticks);
}
