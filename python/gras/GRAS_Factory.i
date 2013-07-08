// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%include <gras/exception.i>
%module (package="gras") GRAS_Factory

%{
#include <gras/factory.hpp>
%}

namespace gras
{
    %ignore Factory::register_make;
    %ignore Factory::make;
}

%newobject gras::Factory::_handle_make;

////////////////////////////////////////////////////////////////////////
// Export swig element comprehension
////////////////////////////////////////////////////////////////////////
%include <std_string.i>
%import <PMC/PMC.i>
%import <gras/element.i>
%include <gras/gras.hpp>
%include <gras/factory.hpp>

////////////////////////////////////////////////////////////////////////
// Create python make method for the factory
////////////////////////////////////////////////////////////////////////
%extend gras::Factory
{
    %insert("python")
    %{
        @staticmethod
        def make(name, *args):
            from PMC import PMC_M
            pmcargs = PMC_M(list(args))
            return Factory._handle_make(name, pmcargs)
    %}
}
