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

////////////////////////////////////////////////////////////////////////
// SWIG director shit - be explicit with all virtual methods
////////////////////////////////////////////////////////////////////////
%module(directors="1") GRAS_Block
%feature("director") gras::BlockPython;
%feature("nodirector") gras::BlockPython::input_buffer_allocator;
%feature("nodirector") gras::BlockPython::output_buffer_allocator;
%feature("nodirector") gras::BlockPython::propagate_tags;
%feature("nodirector") gras::BlockPython::start;
%feature("nodirector") gras::BlockPython::stop;
%feature("nodirector") gras::BlockPython::notify_topology;
%feature("nodirector") gras::BlockPython::work;

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

////////////////////////////////////////////////////////////////////////
// Simple class to deal with smart locking/unlocking of python GIL
////////////////////////////////////////////////////////////////////////
%{

struct PyGILPhondler
{
    PyGILPhondler(void):
        s(PyGILState_Ensure())
    {
        //NOP
    }
    ~PyGILPhondler(void)
    {
        PyGILState_Release(s);
    }
    PyGILState_STATE s;
};

%}

////////////////////////////////////////////////////////////////////////
// SWIG up the representation for IO work arrays
////////////////////////////////////////////////////////////////////////
%include <std_pair.i>
%include <std_vector.i>

%template () std::pair<ptrdiff_t, size_t>;
%template () std::vector<std::pair<ptrdiff_t, size_t> >;

////////////////////////////////////////////////////////////////////////
// Pull in the implementation goodies
////////////////////////////////////////////////////////////////////////
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
    }

    virtual ~BlockPython(void)
    {
        //NOP
    }

    bool start(void)
    {
        PyGILPhondler phil();
        return this->_Py_start();
    }

    virtual bool _Py_start(void) = 0;

    bool stop(void)
    {
        PyGILPhondler phil();
        return this->_Py_stop();
    }

    virtual bool _Py_stop(void) = 0;

    void notify_topology(const size_t num_inputs, const size_t num_outputs)
    {
        PyGILPhondler phil();
        return this->_Py_notify_topology(num_inputs, num_outputs);
    }

    virtual void _Py_notify_topology(const size_t num_inputs, const size_t num_outputs) = 0;

    void work
    (
        const InputItems &input_items,
        const OutputItems &output_items
    )
    {
        _input_items.resize(input_items.size());
        for (size_t i = 0; i < input_items.size(); i++)
        {
            _input_items[i].first = ptrdiff_t(input_items[i].get());
            _input_items[i].second = input_items[i].size();
        }

        _output_items.resize(output_items.size());
        for (size_t i = 0; i < output_items.size(); i++)
        {
            _output_items[i].first = ptrdiff_t(output_items[i].get());
            _output_items[i].second = output_items[i].size();
        }

        PyGILPhondler phil();
        return this->_Py_work(_input_items, _output_items);
    }

    typedef std::vector<std::pair<ptrdiff_t, size_t> > IOPairVec;
    IOPairVec _input_items;
    IOPairVec _output_items;

    virtual void _Py_work
    (
        const IOPairVec &input_items,
        const IOPairVec &output_items
    ) = 0;
};

}

%}

////////////////////////////////////////////////////////////////////////
// Conversion to the numpy array
////////////////////////////////////////////////////////////////////////
%pythoncode %{

import numpy

def pointer_to_ndarray(addr, dtype, nitems, readonly=False):
    class array_like:
        __array_interface__ = {
            'data' : (addr, readonly),
            'typestr' : dtype.base.str,
            'descr' : dtype.base.descr,
            'shape' : (nitems,) + dtype.shape,
            'strides' : None,
            'version' : 3,
        }
    return numpy.asarray(array_like()).view(dtype.base)
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

    def _Py_work(self, input_items, output_items):

        input_arrays = list()
        for i, item in enumerate(input_items):
            addr, nitems = item
            ndarray = pointer_to_ndarray(addr=addr, dtype=self.__in_sig[i], nitems=nitems, readonly=True)
            input_arrays.append(ndarray)

        output_arrays = list()
        for i, item in enumerate(output_items):
            addr, nitems = item
            ndarray = pointer_to_ndarray(addr=addr, dtype=self.__out_sig[i], nitems=nitems, readonly=False)
            output_arrays.append(ndarray)

        ret = self.work(input_arrays, output_arrays)
        if ret is not None:
            raise Exception, 'work return != None, did you call consume/produce?'

    def work(self, *args):
        print 'Implement Work!'

    def _Py_notify_topology(self, *args): return self.notify_topology(*args)
    def notify_topology(self, *args): return

    def _Py_start(self): return self.start()
    def start(self): return True

    def _Py_stop(self): return self.stop()
    def stop(self): return True

%}
