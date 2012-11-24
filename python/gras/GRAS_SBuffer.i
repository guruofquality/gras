// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%include <gras/sbuffer.i>

%{
#include <PMC/PMC.hpp>
%}

%import <PMC/PMC.i>

DECL_PMC_SWIG_TYPE(gras::SBuffer, swig_sbuffer)

%pythoncode %{

from PMC import *

RegisterPy2PMC(
    is_py = lambda x: isinstance(x, SBuffer),
    py2pmc = swig_sbuffer_to_pmc,
)

RegisterPMC2Py(
    is_pmc = pmc_is_swig_sbuffer,
    pmc2py = pmc_to_swig_sbuffer,
)

%}
