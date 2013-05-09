# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import numpy
from demo_blocks import *

class BlockTest(unittest.TestCase):

    def setUp(self):
        self.tb = gras.TopBlock()

    def tearDown(self):
        self.tb = None

    def test_vector_blocks(self):
        vec_source = VectorSource(numpy.uint32, [0, 9, 8, 7, 6])
        vec_sink = VectorSink(numpy.uint32)

        self.tb.connect(vec_source, vec_sink)
        self.tb.run()

        self.assertEqual(vec_sink.get_vector(), (0, 9, 8, 7, 6))

    def test_add_f32(self):
        src0 = VectorSource(numpy.float32, [1, 3, 5, 7, 9])
        src1 = VectorSource(numpy.float32, [0, 2, 4, 6, 8])
        adder = Add2X(numpy.float32)
        sink = VectorSink(numpy.float32)
        self.tb.connect((src0, 0), (adder, 0))
        self.tb.connect((src1, 0), (adder, 1))
        self.tb.connect(adder, sink)
        self.tb.run()
        self.assertEqual(sink.get_vector(), (1, 5, 9, 13, 17))

    def test_add_fc32(self):
        src0 = VectorSource(numpy.complex64, [1, 3j, 5, 7j, 9])
        src1 = VectorSource(numpy.complex64, [0, 2j, 4, 6j, 8])
        adder = Add2X(numpy.complex64)
        sink = VectorSink(numpy.complex64)
        self.tb.connect((src0, 0), (adder, 0))
        self.tb.connect((src1, 0), (adder, 1))
        self.tb.connect(adder, sink)
        self.tb.run()
        self.assertEqual(sink.get_vector(), (1, 5j, 9, 13j, 17))

    def test_add_f32_feedback(self):
        """
        Feedback adder output to input1 of the adder.
        Preload input1 of the adder to set the delay.
        """
        src0 = VectorSource(numpy.float32, [1, 3, 5, 7, 9])
        adder = Add2X(numpy.float32)
        adder.input_config(1).preload_items = 1 #make this a feedback delay of 1
        sink = VectorSink(numpy.float32)
        self.tb.connect((src0, 0), (adder, 0))
        self.tb.connect((adder, 0), (adder, 1))
        self.tb.connect(adder, sink)
        self.tb.run()
        self.assertEqual(sink.get_vector(), (1, 4, 9, 16, 25))

    def test_tag_source_sink(self):
        values = (0, 'hello', 4.2, True, None, [2, 3, 4], (9, 8, 7), 1j, {2:'d'})
        src = TagSource(values)
        sink = TagSink()
        self.tb.connect(src, sink)
        self.tb.run()
        self.assertEqual(sink.get_values(), values)

    def test_ro_buffers(self):
        class BadTouch(gras.Block):
            def __init__(self, in_sig):
                gras.Block.__init__(self, 'BadTouch', in_sig=[in_sig])

            def work(self, ins, outs):
                try:
                    ins[0][0] = 0 #assign to ro buffer should fail
                    raise Exception, 'ins should not be writeable!'
                except: pass
                self.consume(0, len(ins[0]))

        source = VectorSource(numpy.uint32, [0, 9, 8, 7, 6])
        sink = BadTouch(numpy.uint32)

        self.tb.connect(source, sink)
        self.tb.run()

if __name__ == '__main__':
    unittest.main()
