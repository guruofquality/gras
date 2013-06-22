// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%include <PMC/Registry.i>

%include <gras/sbuffer.i>

DECL_PMC_SWIG_TYPE(gras::SBuffer, swig_sbuffer)

%pythoncode %{

def pmc_to_sbuffer(p):
    s = pmc_to_swig_sbuffer(p)
    setattr(s, 'readonly', not isinstance(p, PMC))
    return s

RegisterPy2PMC(
    is_py = lambda x: type(x) is SBuffer,
    py2pmc = swig_sbuffer_to_pmc,
)

RegisterPMC2Py(
    is_pmc = pmc_is_swig_sbuffer,
    pmc2py = pmc_to_sbuffer,
)

%}
