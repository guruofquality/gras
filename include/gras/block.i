// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_BLOCK_I
#define INCLUDED_GRAS_BLOCK_I

%{
#include <gras/block.hpp>
%}

%include <gras/element.i>
%include <gras/detail/property.hpp>
%import <gras/tags.i>
%include <gras/tag_iter.i>
%import <gras/sbuffer.i>
%include <gras/buffer_queue.hpp>
%include <gras/block.hpp>

////////////////////////////////////////////////////////////////////////
// Create pythonic gateway to get and set
////////////////////////////////////////////////////////////////////////
%extend gras::Block
{
    %insert("python")
    %{
        def set(self, key, value):
            self._set_property(key, PMC_M(value))

        def get(self, key):
            return self._get_property(key)()
    %}
}

#endif /*INCLUDED_GRAS_BLOCK_I*/
