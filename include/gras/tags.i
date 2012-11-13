// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_TAGS_I
#define INCLUDED_GRAS_TAGS_I

%{
#include <gras/tags.hpp>
%}

%include <gras/tags.hpp>
%include <PMC/PMC.i>

////////////////////////////////////////////////////////////////////////
// Turn TagIter into a python iterable object
////////////////////////////////////////////////////////////////////////
namespace gras
{
    struct TagIter
    {
        //declared empty
    };
}

%extend gras::TagIter
{
    %insert("python")
    %{
        def __iter__(self):
            for i in range(len(self)):
                yield self[i]
    %}

    size_t __len__(void) const
    {
        return ($self)->size();
    }

    const Tag & __getitem__(const size_t i) const
    {
        return (*($self))[i];
    }
}

#endif /*INCLUDED_GRAS_TAGS_I*/
