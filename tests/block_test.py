
import unittest
import gras
import numpy
from demo_blocks import *

class BlockTest(unittest.TestCase):

    def test_vector_blocks(self):
        vec_source = VectorSource(numpy.uint32, [0, 9, 8, 7, 6])
        vec_sink = VectorSink(numpy.uint32)

        tb = gras.TopBlock()
        tb.connect(vec_source, vec_sink)
        tb.run()
        tb = None

        self.assertEqual(vec_sink.get_vector(), (0, 9, 8, 7, 6))

    def test_add_f32(self):
        tb = gras.TopBlock()
        src0 = VectorSource(numpy.float32, [1, 3, 5, 7, 9])
        src1 = VectorSource(numpy.float32, [0, 2, 4, 6, 8])
        adder = add_2x(numpy.float32)
        sink = VectorSink(numpy.float32)
        tb.connect((src0, 0), (adder, 0))
        tb.connect((src1, 0), (adder, 1))
        tb.connect(adder, sink)
        tb.run()
        tb = None
        self.assertEqual(sink.get_vector(), (1, 5, 9, 13, 17))

if __name__ == '__main__':
    unittest.main()
