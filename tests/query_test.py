# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import numpy
from gras import TestUtils

class MyBlock(gras.Block):
    def __init__(self):
        gras.Block.__init__(self, "MyBlock", out_sig=[numpy.uint32], in_sig=[numpy.uint32])
        self.numeric_value = 0
        self.register_call("get_numeric_value", self.get_numeric_value)
        self.register_call("set_numeric_value", self.set_numeric_value)
        self.vector_value = [0]
        self.register_call("get_vector_value", self.get_vector_value)
        self.register_call("set_vector_value", self.set_vector_value)

    def work(self, ins, outs):
        n = min(len(ins[0]), len(outs[0]))
        outs[0][:n] = ins[0][:n] + self.numeric_value
        self.consume(n)
        self.produce(n)

    def get_numeric_value(self):
        return self.numeric_value

    def set_numeric_value(self, new_numeric_value):
        print "new_numeric_value", new_numeric_value
        self.numeric_value = new_numeric_value

    def get_vector_value(self):
        return self.vector_value

    def set_vector_value(self, new_vector_value):
        print "new_vector_value", new_vector_value
        self.vector_value = numpy.copy(new_vector_value)

class QueryTest(unittest.TestCase):

    def setUp(self):
        self.tb = gras.TopBlock()

    def tearDown(self):
        self.tb = None

    def test_simple(self):
        vec_source = TestUtils.VectorSource(numpy.uint32, [0, 9, 8, 7, 6])
        vec_sink = TestUtils.VectorSink(numpy.uint32)

        self.tb.connect(vec_source, vec_sink)
        self.tb.run()

        self.assertEqual(vec_sink.data(), (0, 9, 8, 7, 6))

        #query the block list
        blocks_result = self.tb.query(dict(path="/blocks.json"))
        self.assertEqual(len(blocks_result['blocks']), 2)

        #pick a block to query below:
        block_id = blocks_result['blocks'].keys()[0]

        #query the stats
        stats_result = self.tb.query(dict(
            path="/stats.json",
            blocks=[block_id],
        ))
        self.assertTrue('tps' in stats_result)
        self.assertTrue('now' in stats_result)

        #found the block we asked for
        self.assertTrue(block_id in stats_result['blocks'])

    def test_numeric_query(self):
        vec_source = TestUtils.VectorSource(numpy.uint32, [0, 9, 8, 7, 6])
        vec_sink = TestUtils.VectorSink(numpy.uint32)
        block = MyBlock()
        block.set_uid("test_numeric_query")
        self.tb.connect(vec_source, block, vec_sink)
        self.tb.run()

        #query the block list
        blocks_result = self.tb.query(dict(path="/blocks.json"))
        self.assertEqual(len(blocks_result['blocks']), 3)
        self.assertTrue('test_numeric_query' in blocks_result['blocks'])

        #set the integer property
        self.tb.query(dict(
            path="/calls.json",
            block='test_numeric_query',
            name='set_numeric_value',
            args=[42],
        ))
        self.assertEqual(block.numeric_value, 42)

        #get the integer property
        block.set_numeric_value(21)
        result = self.tb.query(dict(
            path="/calls.json",
            block='test_numeric_query',
            name='get_numeric_value',
        ))
        self.assertEqual(result['value'], 21)

        #set the complex property
        self.tb.query(dict(
            path="/calls.json",
            block='test_numeric_query',
            name='set_numeric_value',
            args=['(0, 42)'],
        ))
        self.assertEqual(block.numeric_value, 42j)

        #get the complex property
        block.set_numeric_value(21j)
        result = self.tb.query(dict(
            path="/calls.json",
            block='test_numeric_query',
            name='get_numeric_value',
        ))
        self.assertEqual(result['value'], '(0,21)')

    def test_vector_query(self):
        vec_source = TestUtils.VectorSource(numpy.uint32, [0, 9, 8, 7, 6])
        vec_sink = TestUtils.VectorSink(numpy.uint32)
        block = MyBlock()
        block.set_uid("test_vector_query")
        self.tb.connect(vec_source, block, vec_sink)
        self.tb.run()

        #set the vector property
        self.tb.query(dict(
            path="/calls.json",
            block='test_vector_query',
            name='set_vector_value',
            args=[[1, 2, 3, 4, 5]],
        ))
        self.assertEqual(list(block.vector_value), [1, 2, 3, 4, 5])

        #get the vector property
        block.set_vector_value([6, 7, 8, 9])
        result = self.tb.query(dict(
            path="/calls.json",
            block='test_vector_query',
            name='get_vector_value',
        ))
        self.assertEqual(list(result['value']), [6, 7, 8, 9])

if __name__ == '__main__':
    unittest.main()
