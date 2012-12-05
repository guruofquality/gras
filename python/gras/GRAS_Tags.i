// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.


%import <gras/sbuffer.i>
%include <gras/tags.i>

%include <PMC/Registry.i>

DECL_PMC_SWIG_TYPE(gras::StreamTag, swig_stream_tag)
DECL_PMC_SWIG_TYPE(gras::PacketMsg, swig_packet_msg)

%pythoncode %{

from GRAS_SBuffer import SBuffer

RegisterPy2PMC(
    is_py = lambda x: isinstance(x, StreamTag),
    py2pmc = swig_stream_tag_to_pmc,
)

RegisterPMC2Py(
    is_pmc = pmc_is_swig_stream_tag,
    pmc2py = pmc_to_swig_stream_tag,
)

RegisterPy2PMC(
    is_py = lambda x: isinstance(x, PacketMsg),
    py2pmc = swig_packet_msg_to_pmc,
)

RegisterPMC2Py(
    is_pmc = pmc_is_swig_packet_msg,
    pmc2py = pmc_to_swig_packet_msg,
)

%}
