// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/time_tag.hpp>
#include <PMC/Containers.hpp>
#include <boost/cstdint.hpp> //uint64
#include <boost/math/special_functions/round.hpp>

using namespace gras;

static TimeTag &normalize(TimeTag &t)
{
    int num = int(t._ticks/time_tps());
    if (num < 0) num--; //stops negative ticks
    t._fsecs += num;
    t._ticks -= num*time_tps();
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
    t._ticks = boost::math::llround((error*time_tps())/rate);
    return normalize(t);
}

TimeTag TimeTag::from_pmc(const PMCC &p)
{
    TimeTag t;
    const PMCTuple<2> &tuple = p.as<PMCTuple<2> >();
    t._fsecs = tuple[0].as<boost::uint64_t>();
    t._ticks = boost::math::llround(tuple[1].as<double>()*time_tps());
    return normalize(t);
}

time_ticks_t TimeTag::to_ticks(void)
{
    return _fsecs*time_tps() + _ticks;
}

time_ticks_t TimeTag::to_ticks(const double rate)
{
    return _fsecs*time_tps() + boost::math::llround((_ticks*rate)/time_tps());
}

PMCC TimeTag::to_pmc(void)
{
    PMCTuple<2> tuple;
    tuple[0] = PMC_M<boost::uint64_t>(_fsecs);
    tuple[1] = PMC_M<double>(_ticks/double(time_tps()));
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
