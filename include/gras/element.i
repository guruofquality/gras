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
// http://www.swig.org/Doc2.0/Library.html#Library_stl_exceptions
////////////////////////////////////////////////////////////////////////
%include <exception.i>

%exception
{
    try{$action}
    catch (const std::exception& e)
    {SWIG_exception(SWIG_RuntimeError, e.what());}
}

////////////////////////////////////////////////////////////////////////
// Export swig element comprehension
////////////////////////////////////////////////////////////////////////
%include <std_string.i>
%include <gras/gras.hpp>
%include <gras/element.hpp>

////////////////////////////////////////////////////////////////////////
// Operator overloads for Element
////////////////////////////////////////////////////////////////////////
%{

inline bool gras_element_equal(const gras::Element &lhs, const gras::Element &rhs)
{
    return lhs.get() == rhs.get();
}

%}

%extend gras::Element
{
    %insert("python")
    %{
        def __eq__(self, rhs):
            if not isinstance(rhs, Element): return False
            return gras_element_equal(self, rhs)

        def __str__(self):
            return self.to_string()
    %}
}

#endif /*INCLUDED_GRAS_ELEMENT_I*/
