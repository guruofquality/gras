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
%}

////////////////////////////////////////////////////////////////////////
// pull in hier and top interface
////////////////////////////////////////////////////////////////////////
%include <gras/hier_block.i>
%include <gras/top_block.hpp>

////////////////////////////////////////////////////////////////////////
// Make a special top block with python safe unlocking wait
////////////////////////////////////////////////////////////////////////
%include "GRAS_Utils.i"
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

    ~TopBlockPython(void)
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
};

}

%}

////////////////////////////////////////////////////////////////////////
// Remake top block and hier block for multi-arg connnect
////////////////////////////////////////////////////////////////////////
%pythoncode %{

def to_element(obj):
    if isinstance(obj, Element): return obj
    try: return obj.shared_to_element()
    except: raise Exception('cant coerce obj %s to element'%(obj))

def internal_connect__(fcn, obj, *args):

    if len(args) == 1:
        elem = (args[0])
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

class TopBlock(TopBlockPython):
    def __init__(self, name="Top"):
        TopBlockPython.__init__(self, name)

    def connect(self, *args):
        return internal_connect__(TopBlockPython.connect, self, *args)

    def disconnect(self, *args):
        return internal_connect__(TopBlockPython.disconnect, self, *args)

class HierBlock(HierBlockPython):
    def __init__(self, name="Hier"):
        HierBlockPython.__init__(self, name)

    def connect(self, *args):
        return internal_connect__(HierBlockPython.connect, self, *args)

    def disconnect(self, *args):
        return internal_connect__(HierBlockPython.disconnect, self, *args)

%}
