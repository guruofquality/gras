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
// along with io_sig program.  If not, see <http://www.gnu.org/licenses/>.

%rename(io_signature)  gr_make_io_signature;
%rename(io_signature2) gr_make_io_signature2;
%rename(io_signature3) gr_make_io_signature3;
%rename(io_signaturev) gr_make_io_signaturev;

//const size types used by blocks in python
%constant int sizeof_char       = sizeof(char);
%constant int sizeof_short      = sizeof(short);
%constant int sizeof_int        = sizeof(int);
%constant int sizeof_float      = sizeof(float);
%constant int sizeof_double     = sizeof(double);
%constant int sizeof_gr_complex = sizeof(gr_complex);

//helps with funny swig error for io signature
%ignore gnuradio::IOSignature::operator->();
%ignore gnuradio::IOSignature::operator->() const;

%include <gnuradio/element.hpp>
%include <gnuradio/tags.hpp>
%include <gnuradio/block.hpp>
%include <gnuradio/hier_block.hpp>
%include <gnuradio/top_block.hpp>
%include <gnuradio/io_signature.hpp>
%include <gr_io_signature.h>
%include <gr_block.h>
%include <gr_hier_block2.h>
%include <gr_sync_block.h>
%include <gr_sync_decimator.h>
%include <gr_sync_interpolator.h>

////////////////////////////////////////////////////////////////////////
// Make a special top block with python safe unlocking wait
////////////////////////////////////////////////////////////////////////
%include "gruel_common.i"

%inline %{

namespace gnuradio
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
        GR_PYTHON_BLOCKING_CODE
        (
            TopBlock::wait();
        )
    }

    bool wait(const double timeout)
    {
        bool ret = false;
        GR_PYTHON_BLOCKING_CODE
        (
            ret = TopBlock::wait(timeout);
        )
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

top_block = TopBlock

class hier_block(gr_hier_block2):
    def __init__(self, *args, **kwargs):
        gr_hier_block2.__init__(self, *args, **kwargs)
        self._hb = self #backwards compat

    def connect(self, *args):
        return internal_connect__(gr_hier_block2.connect, self, *args)

    def disconnect(self, *args):
        return internal_connect__(gr_hier_block2.disconnect, self, *args)

hier_block2 = hier_block

%}
