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
%feature("director") gras::BlockPython;
%feature("nodirector") gras::BlockPython::input_buffer_allocator;
%feature("nodirector") gras::BlockPython::output_buffer_allocator;
%feature("nodirector") gras::BlockPython::work;
%feature("nodirector") gras::BlockPython::propagate_tags;

////////////////////////////////////////////////////////////////////////
// http://www.swig.org/Doc2.0/Library.html#Library_stl_exceptions
////////////////////////////////////////////////////////////////////////
%include "exception.i"

%exception
{
    try
    {
        $action
    }
    catch (const std::exception& e)
    {
        SWIG_exception(SWIG_RuntimeError, e.what());
    }
}

%feature("director:except")
{
    if ($error != NULL)
    {
        throw Swig::DirectorMethodException();
    }
}

%{
#include <gras/block.hpp>
#include <iostream>
%}

%include <gras/element.i>
%include <gras/io_signature.i>
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
        std::cout << "C++ Block init!!\n";
    }

    virtual ~BlockPython(void){}

    int work
    (
        const InputItems &input_items,
        const OutputItems &output_items
    )
    {
        std::cout << "C++ call work!!\n";
        PyGILState_STATE s = PyGILState_Ensure();
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
    ){}
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
        print 'BlockPython.__init__(self, name)'
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
        return -1

    def check_topology(self, *args):
        print 'check top ', args
        return True

    def start(self):
        print 'PYTHON START'
        return True

    def stop(self):
        print 'PYTHON STOP'
        return False

%}
