// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

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

%{
#include <gras/hier_block.hpp>
#include <gras/top_block.hpp>
#include <boost/make_shared.hpp>
%}

////////////////////////////////////////////////////////////////////////
// pull in hier and top interface
////////////////////////////////////////////////////////////////////////
%import <gras/element.i>
%import <gras/hier_block.i>
%import <gras/top_block.i>

%include "GRAS_Utils.i"

////////////////////////////////////////////////////////////////////////
// weak element overload for python
////////////////////////////////////////////////////////////////////////
%{

struct WeakContainerPyObject : gras::WeakContainer
{
    WeakContainerPyObject(PyObject *o):
        o(o)
    {
        //NOP
    }
    boost::shared_ptr<const void> lock(void)
    {
        return boost::make_shared<PyObjectRefHolder>(o);
    }
    PyObject *o;
};

inline void set_weak_py_self(gras::Element &elem, PyObject *o)
{
    elem.set_container(new WeakContainerPyObject(o));
}

%}

void set_weak_py_self(gras::Element &elem, PyObject *o);

////////////////////////////////////////////////////////////////////////
// Make a special top block with python safe unlocking wait
////////////////////////////////////////////////////////////////////////
%inline %{

namespace gras
{

struct TopBlockPython : TopBlock
{
    TopBlockPython(const std::string &name):
        TopBlock(name)
    {
        //NOP
    }

    virtual ~TopBlockPython(void)
    {
        PyTSPhondler phil;
        this->reset();
    }

    void start(void)
    {
        PyTSPhondler phil;
        TopBlock::start();
    }

    void stop(void)
    {
        PyTSPhondler phil;
        TopBlock::stop();
    }

    void wait(void)
    {
        PyTSPhondler phil;
        TopBlock::wait();
    }

    bool wait(const double timeout)
    {
        PyTSPhondler phil;
        return TopBlock::wait(timeout);
    }

    std::string query(const std::string &args)
    {
        PyTSPhondler phil;
        return TopBlock::query(args);
    }
};

struct HierBlockPython : HierBlock
{
    HierBlockPython(const std::string &name):
        HierBlock(name)
    {
        //NOP
    }

    virtual ~HierBlockPython(void)
    {
        PyTSPhondler phil;
        this->reset();
    }
};

}

%}

////////////////////////////////////////////////////////////////////////
// Remake top block and hier block for multi-arg connnect
////////////////////////////////////////////////////////////////////////
%pythoncode %{

def to_element(obj):
    try:

        #BEGIN basic block support:
        if not hasattr(obj, 'to_element') and hasattr(obj, 'to_basic_block'):
            elem = obj.to_basic_block().to_element()
            set_weak_py_self(elem, obj)
            return elem
        #END basic block support.

        elem = obj.to_element()
        set_weak_py_self(elem, obj)
        return elem
    except: raise Exception('cant coerce obj %s to element'%(obj))

def internal_connect__(fcn, obj, *args):

    if len(args) == 1:
        elem = to_element(args[0])
        fcn(obj, elem)
        return

    for src, sink in zip(args, args[1:]):

        try: src, src_index = src
        except: src_index = 0
        src = to_element(src)

        try: sink, sink_index = sink
        except: sink_index = 0
        sink = to_element(sink)

        fcn(obj, src, src_index, sink, sink_index)

class PyTopBlock(TopBlockPython):
    def __init__(self, name="Top"):
        TopBlockPython.__init__(self, name)

    def connect(self, *args):
        return internal_connect__(TopBlockPython.connect, self, *args)

    def disconnect(self, *args):
        return internal_connect__(TopBlockPython.disconnect, self, *args)

    def query(self, args):
        if isinstance(args, str): return TopBlockPython.query(self, args)
        else:
            import json
            query_str = json.dumps(args)
            result_str = TopBlockPython.query(self, query_str)
            return json.loads(result_str)

class PyHierBlock(HierBlockPython):
    def __init__(self, name="Hier"):
        HierBlockPython.__init__(self, name)

    def connect(self, *args):
        return internal_connect__(HierBlockPython.connect, self, *args)

    def disconnect(self, *args):
        return internal_connect__(HierBlockPython.disconnect, self, *args)

%}
