
import unittest
import gras
import numpy

class BlockTest(unittest.TestCase):

    def test_make_block(self):
        null_src = gras.Block('NullSource')
        null_src.set_output_signature([numpy.int32])

        null_sink = gras.Block('NullSink')
        null_sink.set_input_signature([numpy.int32])

        tb = gras.TopBlock()

        print 'connect...'
        tb.connect(null_src, null_sink)
        print 'ok'

        tb.disconnect(null_src, null_sink)
        print 'done'

if __name__ == '__main__':
    unittest.main()
