// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_ELEMENT_I
#define INCLUDED_GRAS_ELEMENT_I

%module (package="gras") GRAS_Element

%{
#include <gras/element.hpp>
%}

namespace gras
{
    %ignore Element::set_container;
    %ignore Callable::x;
    %ignore Callable::register_call;
}

////////////////////////////////////////////////////////////////////////
// remove base class warning -- boost::shared_ptr<ElementImpl>
////////////////////////////////////////////////////////////////////////
#pragma SWIG nowarn=401

////////////////////////////////////////////////////////////////////////
// Export swig element comprehension
////////////////////////////////////////////////////////////////////////
%include <std_string.i>
%import <PMC/PMC.i>
%include <gras/gras.hpp>
%include <gras/thread_pool.hpp>
%include <gras/block_config.hpp>
%include <gras/callable.hpp>
%include <gras/element.hpp>

////////////////////////////////////////////////////////////////////////
// Operator overloads for Element
////////////////////////////////////////////////////////////////////////
%pythoncode %{
from PMC import *
%}
%extend gras::Element
{
    %insert("python")
    %{
        def x(self, name, *args):
            pmcargs = PMC_M(list(args))
            pmcret = self._handle_call(name, pmcargs)
            return pmcret()

        def __getattr__(self, name):
            return lambda *args: self.x(name, *args)

        def __eq__(self, rhs):
            if not isinstance(rhs, Element): return False
            return self.equals(rhs)

        def __str__(self):
            return self.to_string()
    %}
}

#endif /*INCLUDED_GRAS_ELEMENT_I*/
