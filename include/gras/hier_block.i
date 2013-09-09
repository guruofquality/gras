// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_HIER_BLOCK_I
#define INCLUDED_GRAS_HIER_BLOCK_I

%module (package="gras") GRAS_HierBlock

%{
#include <gras/hier_block.hpp>
%}

%include <gras/gras.hpp>
%import <gras/element.i>
%include <gras/hier_block.hpp>

////////////////////////////////////////////////////////////////////////
// Create pythonic methods
////////////////////////////////////////////////////////////////////////
%pythoncode %{
from PMC import *
%}
%extend gras::HierBlock
{
    %insert("python")
    %{
        def __getattr__(self, name):
            return lambda *args: self.x(name, *args)
    %}
}

#endif /*INCLUDED_GRAS_HIER_BLOCK_I*/
