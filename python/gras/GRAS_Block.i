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

#define GRAS_API

%module(directors="1") GRAS_Block

%feature("director");

%{
#include <gras/block.hpp>
%}

%include <gras/io_signature.i>
%include <gras/element.i>
%include <gras/sbuffer.hpp>
%include <gras/tags.hpp>
%include <gras/block.hpp>
%include <PMC/PMC.i>

%pythoncode %{

import numpy

def sig_to_dtype_sig(sig):
    if sig is None: sig = ()
    return map(numpy.dtype, sig)

class BlockPython(Block):
    def __init__(self, name='Block', in_sig=None, out_sig=None):
        Block.__init__(self, name)
        self.set_input_signature(in_sig)
        self.set_output_signature(in_sig)

    def set_input_signature(self, sig):
        self.__in_sig = sig_to_dtype_sig(sig)
        Block.set_input_signature(self, IOSignature([s.itemsize for s in self.__in_sig]))

    def set_output_signature(self, sig):
        self.__out_sig = sig_to_dtype_sig(sig)
        Block.set_output_signature(self, IOSignature([s.itemsize for s in self.__out_sig]))

    def input_signature(self): return self.__in_sig
    def output_signature(self): return self.__out_sig

%}
