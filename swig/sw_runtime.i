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
};
}

%}

%pythoncode %{

def internal_connect__(fcn, obj, *args):

    if len(args) == 1:
        fcn(obj, args[0].get_base())
        return

    for src, sink in zip(args, args[1:]):
        try: src, src_index = src
        except: src_index = 0
        try: sink, sink_index = sink
        except: sink_index = 0
        fcn(obj, src.get_base(), src_index, sink.get_base(), sink_index)

class top_block(TopBlockPython):
    def __init__(self, *args, **kwargs):
        TopBlockPython.__init__(self, *args, **kwargs)

    def connect(self, *args):
        return internal_connect__(TopBlockPython.connect, self, *args)

    def disconnect(self, *args):
        return internal_connect__(TopBlockPython.disconnect, self, *args)

class hier_block(HierBlock):
    def __init__(self, *args, **kwargs):
        HierBlock.__init__(self, *args, **kwargs)

    def connect(self, *args):
        return internal_connect__(HierBlock.connect, self, *args)

    def disconnect(self, *args):
        return internal_connect__(HierBlock.disconnect, self, *args)

hier_block2 = hier_block

%}
