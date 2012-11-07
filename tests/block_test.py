
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

class VectorSource(gras.Block):
    def __init__(self, out_sig, vec):
        gras.Block.__init__(self,
            name='VectorSource',
            in_sig=None,
            out_sig=[out_sig],
        )
        self._vec = vec

    def work(self, ins, outs):
        print 'vector source work'
        num = min(len(outs[0]), len(self._vec))
        outs[0][:num] = self._vec[:num]
        self.produce(0, num)
        self._vec = self._vec[num:]
        if not self._vec:
            self.mark_done()
        print 'vector source work done'

class VectorSink(gras.Block):
    def __init__(self, in_sig):
        gras.Block.__init__(self,
            name='VectorSink',
            in_sig=[in_sig],
            out_sig=None,
        )
        self._vec = list()

    def get_vector(self):
        return self._vec

    def work(self, ins, outs):
        print 'vector sink work'
        self._vec.extend(ins[0].copy())
        self.consume(0, len(ins[0]))
        print 'vector sink work done'

class BlockTest(unittest.TestCase):

    def test_vector_blocks(self):
        vec_source = VectorSource(numpy.uint32, [0, 9, 8, 7, 6])
        vec_sink = VectorSink(numpy.uint32)

        print 'make tb'
        tb = gras.TopBlock()
        print 'connect'
        tb.connect(vec_source, vec_sink)
        print 'run'
        tb.run()
        print 'done run'
        tb = None

        print vec_sink.get_vector()
        vec_source = None
        vec_sink = None

    def test_make_block(self):
        #return
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
