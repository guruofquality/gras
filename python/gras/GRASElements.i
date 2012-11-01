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

#include <gras/element.hpp>
#include <gras/hier_block.hpp>
#include <gras/top_block.hpp>
#include <gras/io_signature.hpp>

%}


////////////////////////////////////////////////////////////////////////
//helps with funny swig error for io signature
////////////////////////////////////////////////////////////////////////
%ignore gras::IOSignature::operator->();
%ignore gras::IOSignature::operator->() const;

%include <std_vector.i>
%template () std::vector<int>;

////////////////////////////////////////////////////////////////////////
// pull in hier and top interface
////////////////////////////////////////////////////////////////////////
%include <boost_shared_ptr.i>
%shared_ptr(gras::ElementImpl)

%include <gras/element.hpp>
%include <gras/hier_block.hpp>
%include <gras/top_block.hpp>
%include <gras/io_signature.hpp>

////////////////////////////////////////////////////////////////////////
// Make a special top block with python safe unlocking wait
////////////////////////////////////////////////////////////////////////

%inline %{

namespace gras
{

struct TopBlockPython : TopBlock
{
    TopBlockPython(void):
        TopBlock("top")
    {
        //NOP
    }

    TopBlockPython(const std::string &name):
        TopBlock(name)
    {
        //NOP
    }

    void wait(void)
    {
        PyThreadState *s = PyEval_SaveThread();
        TopBlock::wait();
        PyEval_RestoreThread(s);
    }

    bool wait(const double timeout)
    {
        PyThreadState *s = PyEval_SaveThread();
        const bool ret = TopBlock::wait(timeout);
        PyEval_RestoreThread(s);
        return ret;
    }
};
}

%}

////////////////////////////////////////////////////////////////////////
// Remake top block and hier block for multi-arg connnect
////////////////////////////////////////////////////////////////////////
%pythoncode %{

def internal_connect__(fcn, obj, *args):

    def to_element(obj):
        if isinstance(obj, Element): return obj
        try: return obj.shared_to_element()
        except: raise Exception('cant coerce obj %s to element'%(obj))

    if len(args) == 1:
        fcn(obj, to_element(args[0]))
        return

    for src, sink in zip(args, args[1:]):
        try: src, src_index = src
        except: src_index = 0
        try: sink, sink_index = sink
        except: sink_index = 0
        fcn(obj, to_element(src), src_index, to_element(sink), sink_index)

class TopBlock(TopBlockPython):
    def __init__(self, *args, **kwargs):
        TopBlockPython.__init__(self, *args, **kwargs)

    def connect(self, *args):
        return internal_connect__(TopBlockPython.connect, self, *args)

    def disconnect(self, *args):
        return internal_connect__(TopBlockPython.disconnect, self, *args)

class HierBlock(HierBlock):
    def __init__(self, *args, **kwargs):
        HierBlock.__init__(self, *args, **kwargs)
        self._hb = self #backwards compat

    def connect(self, *args):
        return internal_connect__(HierBlock.connect, self, *args)

    def disconnect(self, *args):
        return internal_connect__(HierBlock.disconnect, self, *args)

%}
