// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_TOP_BLOCK_I
#define INCLUDED_GRAS_TOP_BLOCK_I

%module (package="gras") GRAS_TopBlock

%{
#include <gras/top_block.hpp>
%}

%include <gras/gras.hpp>
%import <gras/element.i>
%import <gras/hier_block.i>
%include <gras/top_block.hpp>

#endif /*INCLUDED_GRAS_TOP_BLOCK_I*/
