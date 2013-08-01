// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

////////////////////////////////////////////////////////////////////////
// SWIG director shit - be explicit with all virtual methods
////////////////////////////////////////////////////////////////////////
%module(directors="1") GRAS_Block
%feature("director") gras::BlockPython;
%feature("nodirector") gras::BlockPython::input_buffer_allocator;
%feature("nodirector") gras::BlockPython::output_buffer_allocator;
%feature("nodirector") gras::BlockPython::propagate_tags;
%feature("nodirector") gras::BlockPython::notify_active;
%feature("nodirector") gras::BlockPython::notify_inactive;
%feature("nodirector") gras::BlockPython::notify_topology;
%feature("nodirector") gras::BlockPython::work;
%feature("nodirector") gras::BlockPython::_handle_call_ts;

////////////////////////////////////////////////////////////////////////
// http://www.swig.org/Doc2.0/Library.html#Library_stl_exceptions
////////////////////////////////////////////////////////////////////////
%include <exception.i>

%exception
{
    try
    {
        $action
    }
    catch (const Swig::DirectorException &)
    {
        SWIG_fail;
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
%}

////////////////////////////////////////////////////////////////////////
// SWIG up the representation for IO work arrays
////////////////////////////////////////////////////////////////////////
%include <std_vector.i>
%template () std::vector<size_t>;
%template () std::vector<void *>;

////////////////////////////////////////////////////////////////////////
// Pull in the implementation goodies
////////////////////////////////////////////////////////////////////////
%import <gras/element.i>
%include <gras/tag_iter.i>
%import <gras/block.i>

%include "GRAS_Utils.i"

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
        PyTSPhondler phil;
        this->reset();
    }

    void notify_active(void)
    {
        PyGILPhondler phil;
        return this->_Py_notify_active();
    }

    virtual void _Py_notify_active(void) = 0;

    void notify_inactive(void)
    {
        PyGILPhondler phil;
        return this->_Py_notify_inactive();
    }

    virtual void _Py_notify_inactive(void) = 0;

    void notify_topology(const size_t num_inputs, const size_t num_outputs)
    {
        _input_addrs.resize(num_inputs);
        _input_sizes.resize(num_inputs);
        _output_addrs.resize(num_outputs);
        _output_sizes.resize(num_outputs);

        PyGILPhondler phil;
        return this->_Py_notify_topology(num_inputs, num_outputs);
    }

    virtual void _Py_notify_topology(const size_t, const size_t) = 0;

    void work
    (
        const InputItems &input_items,
        const OutputItems &output_items
    )
    {
        for (size_t i = 0; i < input_items.size(); i++)
        {
            _input_addrs[i] = (void *)(input_items[i].get());
            _input_sizes[i] = input_items[i].size();
        }

        for (size_t i = 0; i < output_items.size(); i++)
        {
            _output_addrs[i] = (void *)(output_items[i].get());
            _output_sizes[i] = output_items[i].size();
        }

        PyGILPhondler phil;
        return this->_Py_work(_input_addrs, _input_sizes, _output_addrs, _output_sizes);
    }

    std::vector<void *> _input_addrs;
    std::vector<size_t> _input_sizes;
    std::vector<void *> _output_addrs;
    std::vector<size_t> _output_sizes;

    virtual void _Py_work
    (
        const std::vector<void *> &,
        const std::vector<size_t> &,
        const std::vector<void *> &,
        const std::vector<size_t> &
    ) = 0;

    void propagate_tags(const size_t which_input, const TagIter &iter)
    {
        PyGILPhondler phil;
        return this->_Py_propagate_tags(which_input, iter);
    }

    virtual void _Py_propagate_tags(const size_t which_input, const TagIter &iter) = 0;

    PMCC _handle_call(const std::string &name, const PMCC &args)
    {
        PyTSPhondler phil;
        return Block::_handle_call(name, args);
    }

    PMCC _handle_call_ts(const std::string &name, const PMCC &args)
    {
        PyGILPhondler phil;
        return this->_Py_handle_call_ts(name, args);
    }

    virtual PMCC _Py_handle_call_ts(const std::string &name, const PMCC &args) = 0;

    //dummy registration so the C++ knows at least the name names
    void dummy_register_call(const std::string &name)
    {
        this->register_call(name, &BlockPython::__my_dummy);
    }

    //dummy call that should not really be called!
    void __my_dummy(void)
    {
        throw std::runtime_error("BlockPython dummy method called -- should not happen!");
    }
};

}

%}

////////////////////////////////////////////////////////////////////////
// Python overload for adding pythonic interfaces
////////////////////////////////////////////////////////////////////////
%pythoncode %{

import numpy
import traceback
from GRAS_Utils import pointer_to_ndarray
from PMC import *

def sig_to_dtype_sig(sig):
    if sig is None: sig = ()
    return map(numpy.dtype, sig)

class PyBlock(BlockPython):
    def __init__(self, name='Block', in_sig=None, out_sig=None):
        BlockPython.__init__(self, name)
        self.set_input_signature(in_sig)
        self.set_output_signature(out_sig)
        self.__call_registry = dict()

    def set_input_signature(self, sig):
        self.__in_sig = sig_to_dtype_sig(sig)
        for i, n in enumerate(self.__in_sig): self.input_config(i).item_size = n.itemsize

    def set_output_signature(self, sig):
        self.__out_sig = sig_to_dtype_sig(sig)
        for i, n in enumerate(self.__out_sig): self.output_config(i).item_size = n.itemsize

    def input_signature(self): return self.__in_sig
    def output_signature(self): return self.__out_sig

    def _Py_work(self, input_addrs, input_sizes, output_addrs, output_sizes):

        try:

            input_arrays = list()
            for i in self.__in_indexes:
                addr = long(input_addrs[i])
                nitems = input_sizes[i]
                ndarray = pointer_to_ndarray(addr=addr, dtype=self.__in_sig[i], nitems=nitems, readonly=True)
                input_arrays.append(ndarray)

            output_arrays = list()
            for i in self.__out_indexes:
                addr = long(output_addrs[i])
                nitems = output_sizes[i]
                ndarray = pointer_to_ndarray(addr=addr, dtype=self.__out_sig[i], nitems=nitems, readonly=False)
                output_arrays.append(ndarray)

            ret = self.work(input_arrays, output_arrays)
            if ret is not None:
                raise Exception, 'work return != None, did you call consume/produce?'
        except: traceback.print_exc(); raise

    def work(self, *args):
        print 'Implement Work!'

    def _Py_notify_topology(self, num_inputs, num_outputs):

        #extend input signature from back to match topology
        while len(self.__in_sig) < num_inputs:
            self.__in_sig.append(self.__in_sig[-1])

        #extend output signature from back to match topology
        while len(self.__out_sig) < num_outputs:
            self.__out_sig.append(self.__out_sig[-1])

        self.__in_indexes = range(num_inputs)
        self.__out_indexes = range(num_outputs)
        try: return self.notify_topology(num_inputs, num_outputs)
        except: traceback.print_exc(); raise

    def notify_topology(self, *args): return

    def _Py_notify_active(self):
        try: return self.notify_active()
        except: traceback.print_exc(); raise

    def notify_active(self): pass

    def _Py_notify_inactive(self):
        try: return self.notify_inactive()
        except: traceback.print_exc(); raise

    def notify_inactive(self): pass

    def _Py_propagate_tags(self, which_input, iter):
        try: return self.propagate_tags(which_input, iter)
        except: traceback.print_exc(); raise

    def propagate_tags(self, i, iter):
        for o in self.__out_indexes:
            for t in iter:
                t.offset += self.get_produced(o)
                t.offset -= self.get_consumed(i)
                self.post_output_tag(o, t)

    def _Py_handle_call_ts(self, name, args):
        call = self.__call_registry[name]
        pyargs = args()
        pyret = call(*pyargs)
        return PMC_M(pyret)

    def register_call(self, name, call):
        self.dummy_register_call(name) #c++ knows name name
        self.__call_registry[name] = call
%}
