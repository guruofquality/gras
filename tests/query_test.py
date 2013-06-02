# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import numpy
from demo_blocks import *

class MyBlock(gras.Block):
    def __init__(self):
        gras.Block.__init__(self, "MyBlock", out_sig=[numpy.uint32], in_sig=[numpy.uint32])
        self.foo = 0
        self.register_getter("foo", self.get_foo)
        self.register_setter("foo", self.set_foo)

    def work(self, ins, outs):
        n = min(len(ins[0]), len(outs[0]))
        outs[0][:n] = ins[0][:n] + self.foo
        self.consume(n)
        self.produce(n)

    def get_foo(self):
        return self.foo

    def set_foo(self, new_foo):
        print "new_foo", new_foo
        new_foo + 0 #throws if its not a number
        self.foo = new_foo

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

    def test_props(self):
        vec_source = VectorSource(numpy.uint32, [0, 9, 8, 7, 6])
        vec_sink = VectorSink(numpy.uint32)
        block = MyBlock()
        block.set_uid("my_block_uid")
        self.tb.connect(vec_source, block, vec_sink)
        self.tb.run()

        #query the block list
        blocks_result = self.tb.query(dict(path="/blocks.json"))
        self.assertEqual(len(blocks_result['blocks']), 3)
        self.assertTrue('my_block_uid' in blocks_result['blocks'])

        #set the property

        #get the property

if __name__ == '__main__':
    unittest.main()
