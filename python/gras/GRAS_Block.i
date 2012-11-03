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

%feature("director") BlockPython;

%{
#include <gras/block.hpp>
%}

%include <gras/io_signature.i>
%include <gras/element.i>
%include <gras/sbuffer.hpp>
%include <gras/tags.hpp>
%include <gras/block.hpp>
%include <PMC/PMC.i>


////////////////////////////////////////////////////////////////////////
// Make a special block with safe overloads
////////////////////////////////////////////////////////////////////////

%inline %{

namespace gras
{

struct BlockPython : Block
{
    BlockPython(const std::string &name):
        Block(name)
    {
        //NOP
    }

    int work
    (
        const InputItems &input_items,
        const OutputItems &output_items
    )
    {
        PyGILState_STATE s;
        s = PyGILState_Ensure();
        int ret = 0;
        try
        {
            ret = this->python_work(input_items, output_items);
        }
        catch(...)
        {
            PyGILState_Release(s);
            throw;
        }
        PyGILState_Release(s);
        return ret;
    }

    virtual int python_work
    (
        const InputItems &input_items,
        const OutputItems &output_items
    )
    {
        throw std::runtime_error("Error in BlockPython::python_work: SWIG directors issue?");
    }
};

}

%}

////////////////////////////////////////////////////////////////////////
// Python overload for adding pythonic interfaces
////////////////////////////////////////////////////////////////////////
%pythoncode %{

import numpy

def sig_to_dtype_sig(sig):
    if sig is None: sig = ()
    return map(numpy.dtype, sig)

class Block(BlockPython):
    def __init__(self, name='Block', in_sig=None, out_sig=None):
        BlockPython.__init__(self, name)
        self.set_input_signature(in_sig)
        self.set_output_signature(in_sig)

    def set_input_signature(self, sig):
        self.__in_sig = sig_to_dtype_sig(sig)
        BlockPython.set_input_signature(self, IOSignature([s.itemsize for s in self.__in_sig]))

    def set_output_signature(self, sig):
        self.__out_sig = sig_to_dtype_sig(sig)
        BlockPython.set_output_signature(self, IOSignature([s.itemsize for s in self.__out_sig]))

    def input_signature(self): return self.__in_sig
    def output_signature(self): return self.__out_sig

    def python_work(self, input_items, output_items):
        print 'python work called'

%}
