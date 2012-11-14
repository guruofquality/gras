// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_IO_SIGNATURE_I
#define INCLUDED_GRAS_IO_SIGNATURE_I

%{
#include <gras/io_signature.hpp>
%}

%ignore gras::IOSignature::operator->();
%ignore gras::IOSignature::operator->() const;

%include <std_vector.i>
%template () std::vector<size_t>;

%ignore gras::IOSignature::operator[]; //ignore warnings about %extend

%include <gras/io_signature.hpp>

%extend gras::IOSignature
{
    const size_t &__getitem__(const size_t index)
    {
        return ($self)->at(index);
    }
}

#endif /*INCLUDED_GRAS_IO_SIGNATURE_I*/
