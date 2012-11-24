// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_IO_SIGNATURE_I
#define INCLUDED_GRAS_IO_SIGNATURE_I

%{
#include <gras/io_signature.hpp>
%}

%ignore gras::IOSignature::operator->();
%ignore gras::IOSignature::operator->() const;
%ignore gras::IOSignature::operator[]; //ignore warnings about %extend

%include <std_vector.i>
%template (std_vector_gras_io_signature_unsigned) std::vector<unsigned>;

%include <gras/io_signature.hpp>

%extend gras::IOSignature
{
    const unsigned &__getitem__(const unsigned index)
    {
        return ($self)->at(index);
    }
}

#endif /*INCLUDED_GRAS_IO_SIGNATURE_I*/
