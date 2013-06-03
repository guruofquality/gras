# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import numpy
from demo_blocks import *

class MyBlock(gras.Block):
    def __init__(self):
        gras.Block.__init__(self, "MyBlock", out_sig=[numpy.uint32], in_sig=[numpy.uint32])
        self.integer_value = 0
        self.register_getter("integer_value", self.get_integer_value)
        self.register_setter("integer_value", self.set_integer_value)
        self.vector_value = [0]
        self.register_getter("vector_value", self.get_vector_value)
        self.register_setter("vector_value", self.set_vector_value)

    def work(self, ins, outs):
        n = min(len(ins[0]), len(outs[0]))
        outs[0][:n] = ins[0][:n] + self.integer_value
        self.consume(n)
        self.produce(n)

    def get_integer_value(self):
        return self.integer_value

    def set_integer_value(self, new_integer_value):
        print "new_integer_value", new_integer_value
        self.integer_value = new_integer_value

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
        vec_source = VectorSource(numpy.uint32, [0, 9, 8, 7, 6])
        vec_sink = VectorSink(numpy.uint32)

        self.tb.connect(vec_source, vec_sink)
        self.tb.run()

        self.assertEqual(vec_sink.get_vector(), (0, 9, 8, 7, 6))

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

    def test_integer_query(self):
        vec_source = VectorSource(numpy.uint32, [0, 9, 8, 7, 6])
        vec_sink = VectorSink(numpy.uint32)
        block = MyBlock()
        block.set_uid("test_integer_query")
        self.tb.connect(vec_source, block, vec_sink)
        self.tb.run()

        #query the block list
        blocks_result = self.tb.query(dict(path="/blocks.json"))
        self.assertEqual(len(blocks_result['blocks']), 3)
        self.assertTrue('test_integer_query' in blocks_result['blocks'])

        #set the integer property
        self.tb.query(dict(
            path="/props.json",
            block='test_integer_query',
            key='integer_value',
            action='set',
            value=42,
        ))
        self.assertEqual(block.integer_value, 42)

        #get the integer property
        block.set('integer_value', 21)
        result = self.tb.query(dict(
            path="/props.json",
            block='test_integer_query',
            key='integer_value',
            action='get'
        ))
        self.assertEqual(result['value'], 21)

    def test_vector_query(self):
        vec_source = VectorSource(numpy.uint32, [0, 9, 8, 7, 6])
        vec_sink = VectorSink(numpy.uint32)
        block = MyBlock()
        block.set_uid("test_vector_query")
        self.tb.connect(vec_source, block, vec_sink)
        self.tb.run()

        #set the vector property
        self.tb.query(dict(
            path="/props.json",
            block='test_vector_query',
            key='vector_value',
            action='set',
            value=[1, 2, 3, 4, 5],
        ))
        print block.vector_value
        self.assertEqual(list(block.vector_value), [1, 2, 3, 4, 5])

if __name__ == '__main__':
    unittest.main()
