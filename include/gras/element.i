// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_ELEMENT_I
#define INCLUDED_GRAS_ELEMENT_I

%{
#include <gras/element.hpp>
%}

////////////////////////////////////////////////////////////////////////
// remove base class warning
////////////////////////////////////////////////////////////////////////
#pragma SWIG nowarn=401

////////////////////////////////////////////////////////////////////////
// Export swig element comprehension
////////////////////////////////////////////////////////////////////////
%include <std_string.i>
%include <gras/gras.hpp>
%include <gras/io_signature.i>
%include <gras/element.hpp>

////////////////////////////////////////////////////////////////////////
// Operator overloads for Element
////////////////////////////////////////////////////////////////////////
%extend gras::Element
{
    std::string __str__(void) const
    {
       return ($self)->to_string();
    }

    bool _equal(const Element &rhs) const
    {
        return ($self)->get() == rhs.get();
    }

    %insert("python")
    %{
        def __eq__(self, rhs):
            if not isinstance(rhs, Element): return False
            return self._equal(rhs)
    %}
}

#endif /*INCLUDED_GRAS_ELEMENT_I*/
