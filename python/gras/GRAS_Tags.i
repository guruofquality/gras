// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.


%import <gras/sbuffer.i>
%include <gras/tags.i>

%include <PMC/Registry.i>

DECL_PMC_SWIG_TYPE(gras::StreamTag, swig_stream_tag)
DECL_PMC_SWIG_TYPE(gras::PacketMsg, swig_packet_msg)

%pythoncode %{
from GRAS_Tags import StreamTag, PacketMsg
%}

REG_PMC_SWIG_TYPE(swig_stream_tag, StreamTag)
REG_PMC_SWIG_TYPE(swig_packet_msg, PacketMsg)
