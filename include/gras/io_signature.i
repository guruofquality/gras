// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_IO_SIGNATURE_I
#define INCLUDED_GRAS_IO_SIGNATURE_I

%{
#include <gras/io_signature.hpp>
%}

%ignore gras::IOSignature::operator->();
%ignore gras::IOSignature::operator->() const;

%include <std_vector.i>
%template () std::vector<int>;

%include <gras/io_signature.hpp>

#endif /*INCLUDED_GRAS_IO_SIGNATURE_I*/
