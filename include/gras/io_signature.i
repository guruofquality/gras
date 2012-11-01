//
// Copyright 2012 Josh Blum
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
