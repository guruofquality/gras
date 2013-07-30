# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import gras
import numpy
from PMC import *

class NullSource(gras.Block):
    def __init__(self, out_sig):
        gras.Block.__init__(self, 'NullSource', out_sig=[out_sig])

    def work(self, ins, outs):
        outs[0][:] = numpy.zeros(len(outs[0]))
        self.produce(0, len(outs[0]))

class NullSink(gras.Block):
    def __init__(self, in_sig):
        gras.Block.__init__(self, 'NullSink', in_sig=[in_sig])

    def work(self, ins, outs):
        self.consume(0, len(ins[0]))

class VectorSource(gras.Block):
    def __init__(self, out_sig, vec):
        gras.Block.__init__(self, name='VectorSource', out_sig=[out_sig])
        self._vec = vec

    def work(self, ins, outs):
        num = min(len(outs[0]), len(self._vec))
        outs[0][:num] = self._vec[:num]
        self.produce(0, num)
        self._vec = self._vec[num:]
        if not len(self._vec):
            self.mark_done()

class VectorSink(gras.Block):
    def __init__(self, in_sig):
        gras.Block.__init__(self, name='VectorSink', in_sig=[in_sig])
        self._vec = list()

    def data(self):
        return tuple(self._vec)

    def work(self, ins, outs):
        self._vec.extend(ins[0].copy())
        self.consume(0, len(ins[0]))

class Head(gras.Block):
    def __init__(self, sig, num_items):
        gras.Block.__init__(self, name='Head', in_sig=[sig], out_sig=[sig])
        self._num_items = num_items

    def work(self, ins, outs):
        n = min(len(ins[0]), len(outs[0]), self._num_items)
        outs[0][:n] = ins[0][:n]
        self.consume(0, n)
        self.produce(0, n)
        self._num_items -= n
        if not self._num_items:
            self.mark_done()

class Add2X(gras.Block):
    def __init__(self, sig):
        gras.Block.__init__(self,
            name = "Add2X",
            in_sig = [sig, sig],
            out_sig = [sig],
        )

    def work(self, ins, outs):
        nitems = min(*map(len, (ins[0], ins[1], outs[0])))
        outs[0][:nitems] = ins[0][:nitems] + ins[1][:nitems]
        self.consume(0, nitems)
        self.consume(1, nitems)
        self.produce(0, nitems)

class TagSource(gras.Block):
    def __init__(self, values):
        gras.Block.__init__(self,
            name = "TagSource",
            out_sig = [numpy.uint8],
        )
        self._values = values

    def work(self, ins, outs):
        offset = self.get_produced(0)
        tag = gras.Tag(offset, PMC_M(self._values[0]))
        self.post_output_tag(0, tag)
        self.produce(0, len(outs[0]))
        self._values = self._values[1:]
        if not self._values:
            self.mark_done()

class TagSink(gras.Block):
    def __init__(self):
        gras.Block.__init__(self,
            name = "TagSink",
            in_sig = [numpy.uint8],
        )
        self._values = list()

    def get_values(self):
        return tuple(self._values)

    def work(self, ins, outs):
        max_read = self.get_consumed(0) + len(ins[0])
        for tag in self.get_input_tags(0):
            if tag.offset < max_read:
                self._values.append(tag.object())
        self.consume(0, len(ins[0]))
