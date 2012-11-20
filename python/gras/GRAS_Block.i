// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

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
    catch (const Swig::DirectorException &e)
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
%include <std_vector.i>
%template () std::vector<size_t>;
%template () std::vector<void *>;

////////////////////////////////////////////////////////////////////////
// Pull in the implementation goodies
////////////////////////////////////////////////////////////////////////
%{
#include <gras/block.hpp>
#include <iostream>
%}

%include <gras/block.i>

////////////////////////////////////////////////////////////////////////
// Support for the ref counted container
////////////////////////////////////////////////////////////////////////
%{
#include <boost/make_shared.hpp>
%}

%inline %{

namespace gras
{

struct PyObjectRefHolder
{
    PyObjectRefHolder(PyObject *o):
        o(o)
    {
        Py_INCREF(o);
    }
    ~PyObjectRefHolder(void)
    {
        Py_DECREF(o);
    }
    PyObject *o;
};

struct WeakElementPyObject : WeakElement
{
    WeakElementPyObject(PyObject *o):
        o(o)
    {
        //NOP
    }
    boost::shared_ptr<void> lock(void)
    {
        return boost::make_shared<PyObjectRefHolder>(o);
    }
    PyObject *o;
};

}

%}

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

    void _Py_set_ref(PyObject *o)
    {
        this->weak_self.reset(new WeakElementPyObject(o));
    }

    bool start(void)
    {
        PyGILPhondler phil;
        return this->_Py_start();
    }

    virtual bool _Py_start(void) = 0;

    bool stop(void)
    {
        PyGILPhondler phil;
        return this->_Py_stop();
    }

    virtual bool _Py_stop(void) = 0;

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
import traceback

def sig_to_dtype_sig(sig):
    if sig is None: sig = ()
    return map(numpy.dtype, sig)

Tag__ = Tag

class Tag(object):
    def __init__(self, offset=0, key=None, value=None, srcid=None):
        self.offset = offset
        self.key = key
        self.value = value
        self.srcid = srcid

def YieldTagIter(iter):
    for t in iter: yield Tag(
        offset=t.offset,
        key=PMC2Py(t.key),
        value=PMC2Py(t.value),
        srcid=PMC2Py(t.srcid),
    )

class Block(BlockPython):
    def __init__(self, name='Block', in_sig=None, out_sig=None):
        BlockPython.__init__(self, name)
        self.set_input_signature(in_sig)
        self.set_output_signature(out_sig)

        '''
        Setup reference counting foo:
        This logic allows a topology to hold a container reference.
        However, getting this to actually work was quite a adventure.
        We are using the reference to this class's internal dictionary;
        but I would prefer that the reference be to this PyObject.
        Anyway, this seems to be working properly, but could be fragile.
        '''

        # 1) give the element a PyObject ptr to this elements dict
        # 2) set self as a member so the dict hold a reference to self
        self._Py_set_ref(self)
        self.__ref = self

    def set_input_signature(self, sig):
        self.__in_sig = sig_to_dtype_sig(sig)
        BlockPython.set_input_signature(self, IOSignature([s.itemsize for s in self.__in_sig]))

    def set_output_signature(self, sig):
        self.__out_sig = sig_to_dtype_sig(sig)
        BlockPython.set_output_signature(self, IOSignature([s.itemsize for s in self.__out_sig]))

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
        self.__in_indexes = range(num_inputs)
        self.__out_indexes = range(num_outputs)
        try: return self.notify_topology(num_inputs, num_outputs)
        except: traceback.print_exc(); raise

    def notify_topology(self, *args): return

    def _Py_start(self):
        try: return self.start()
        except: traceback.print_exc(); raise

    def start(self): return True

    def _Py_stop(self):
        try: return self.stop()
        except: traceback.print_exc(); raise

    def stop(self): return True

    def post_output_tag(self, which_output, tag):
        t = Tag__(
            tag.offset,
            Py2PMC(tag.key),
            Py2PMC(tag.value),
            Py2PMC(tag.srcid),
        )
        BlockPython.post_output_tag(self, which_output, t)

    def get_input_tags(self, which_input):
        return YieldTagIter(BlockPython.get_input_tags(self, which_input))

    def _Py_propagate_tags(self, which_input, iter):
        try: return self.propagate_tags(which_input, iter)
        except: traceback.print_exc(); raise

    def propagate_tags(self, i, iter):
        for o in self.__out_indexes:
            for t in YieldTagIter(iter):
                t.offset -= self.get_consumed(i)
                t.offset += self.get_produced(o)
                self.post_output_tag(o, t)

%}
