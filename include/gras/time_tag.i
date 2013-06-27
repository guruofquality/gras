// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_TIME_TAG_I
#define INCLUDED_GRAS_TIME_TAG_I

%module (package="gras") GRAS_TimeTag

%{
#include <gras/time_tag.hpp>
%}

////////////////////////////////////////////////////////////////////////
// remove base class warning -- boost::less_than_comparable<TimeTag>
// remove base class warning -- boost::additive<TimeTag>
////////////////////////////////////////////////////////////////////////
#pragma SWIG nowarn=401

%include <gras/gras.hpp>
%include <gras/chrono.hpp>
%include <gras/time_tag.hpp>
%import <PMC/PMC.i>

////////////////////////////////////////////////////////////////////////
// Make it pythonic
////////////////////////////////////////////////////////////////////////
%extend gras::TimeTag
{
    bool __nonzero__(void)
    {
        return ($self)->to_ticks() != 0;
    }

    int __cmp__(const TimeTag &other)
    {
        if ((*($self)) < other) return -1;
        if ((*($self)) > other) return +1;
        return 0;
    }

    TimeTag __add__(const TimeTag &other)
    {
        return (*($self)) + other;
    }

    TimeTag __sub__(const TimeTag &other)
    {
        return (*($self)) - other;
    }
}

#endif /*INCLUDED_GRAS_TIME_TAG_I*/
