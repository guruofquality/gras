// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_TAGS_I
#define INCLUDED_GRAS_TAGS_I

%module (package="gras") GRAS_Tags

%{
#include <gras/tags.hpp>
%}

////////////////////////////////////////////////////////////////////////
// remove base class warning -- boost::less_than_comparable<Tag>
////////////////////////////////////////////////////////////////////////
#pragma SWIG nowarn=401

%include <gras/gras.hpp>
%include <gras/tags.hpp>
%import <gras/sbuffer.i>
%import <PMC/PMC.i>

#endif /*INCLUDED_GRAS_TAGS_I*/
