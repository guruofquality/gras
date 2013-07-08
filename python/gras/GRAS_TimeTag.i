// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%include <PMC/Registry.i>

%include <gras/time_tag.i>

DECL_PMC_SWIG_TYPE(gras::TimeTag, swig_time_tag)

%pythoncode %{
from GRAS_TimeTag import TimeTag
%}

REG_PMC_SWIG_TYPE(swig_time_tag, TimeTag)
