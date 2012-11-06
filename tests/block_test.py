
import unittest
import gras
import numpy

class NullSource(gras.Block):
    def __init__(self):
        gras.Block.__init__(self, 'NullSource')
        self.set_output_signature([numpy.int32])

    def work(self, ins, outs):
        self.mark_done()

class NullSink(gras.Block):
    def __init__(self):
        gras.Block.__init__(self, 'NullSink')
        self.set_input_signature([numpy.int32])

    def work(self, ins, outs):
        self.mark_done()

class BlockTest(unittest.TestCase):

    def test_make_block(self):
        null_src = NullSource()

        null_sink = NullSink()

        tb = gras.TopBlock()

        print 'connect...'
        tb.connect(null_src, null_sink)
        print 'run'

        print 'py start'
        tb.start()
        import time; time.sleep(1)
        print 'py stop'
        tb.stop()
        print 'py wait'
        tb.wait()
        print 'ok'

        tb.disconnect(null_src, null_sink)
        tb.commit()
        print 'done'

if __name__ == '__main__':
    unittest.main()
