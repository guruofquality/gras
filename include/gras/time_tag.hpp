// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_TIME_TAG_HPP
#define INCLUDED_GRAS_TIME_TAG_HPP

#include <gras/gras.hpp>
#include <gras/chrono.hpp>
#include <PMC/PMC.hpp>
#include <boost/operators.hpp>

namespace gras
{

/*!
 * TimeTag represents an absolute time or a time delta.
 * A TimeTag can be converted to and from a tick count.
 * Conversion support is provided for the pseudo-standard
 * PMCTuple format often used inside a StreamTag value.
 * And TimeTag supports overloaded arithmetic operations.
 */
struct GRAS_API TimeTag :
    boost::less_than_comparable<TimeTag>,
    boost::additive<TimeTag>
{
    //! Default contructor - hold time 0
    TimeTag(void);

    //! Create a time tag from ticks w/ default ticks per second
    static TimeTag from_ticks(const time_ticks_t ticks);

    //! Create a time tag from ticks w/ specified ticks per second
    static TimeTag from_ticks(const time_ticks_t ticks, const double rate);

    //! Create a time tag from a PMC containing a PMCTuple<2>(uint64, double)
    static TimeTag from_pmc(const PMCC &p);

    //! Convert this time tag to ticks w/ default ticks per second
    time_ticks_t to_ticks(void) const;

    //! Convert this time tag to ticks w/ specified ticks per second
    time_ticks_t to_ticks(const double rate) const;

    //! Convert this time tag to a PMC containing a PMCTuple<2>(uint64, double)
    PMCC to_pmc(void) const;

    //! Addition for additive interface
    TimeTag &operator+=(const TimeTag &);

    //! Subtraction for additive interface
    TimeTag &operator-=(const TimeTag &);

    //! full seconds
    time_ticks_t _fsecs;

    //! fractional ticks
    time_ticks_t _ticks;
};

GRAS_API bool operator<(const TimeTag &lhs, const TimeTag &rhs);

GRAS_API bool operator==(const TimeTag &lhs, const TimeTag &rhs);

} //namespace gras

#endif /*INCLUDED_GRAS_TIME_TAG_HPP*/
