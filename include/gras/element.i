// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_ELEMENT_I
#define INCLUDED_GRAS_ELEMENT_I

%module GRAS_Element

%{
#include <gras/element.hpp>
%}

namespace gras
{
    %ignore Element::set_container;
}

////////////////////////////////////////////////////////////////////////
// remove base class warning -- boost::shared_ptr<ElementImpl>
////////////////////////////////////////////////////////////////////////
#pragma SWIG nowarn=401

////////////////////////////////////////////////////////////////////////
// Export swig element comprehension
////////////////////////////////////////////////////////////////////////
%include <std_string.i>
%include <gras/gras.hpp>
%include <gras/exception.i>
%include <gras/element.hpp>

////////////////////////////////////////////////////////////////////////
// Operator overloads for Element
////////////////////////////////////////////////////////////////////////
%extend gras::Element
{
    %insert("python")
    %{
        def __eq__(self, rhs):
            if not isinstance(rhs, Element): return False
            return self.equals(rhs)

        def __str__(self):
            return self.to_string()
    %}
}

#endif /*INCLUDED_GRAS_ELEMENT_I*/
