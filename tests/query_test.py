# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import numpy
from demo_blocks import *
import json

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

        blocks_json = self.tb.query("<args><path>/blocks.json</path></args>")
        print blocks_json
        blocks_python = json.loads(blocks_json)
        print blocks_python
        self.assertEqual(len(blocks_python['blocks']), 2)

        stats_json = self.tb.query("<args><path>/stats.json</path></args>")
        print stats_json
        stats_python = json.loads(stats_json)
        print stats_python
        self.assertTrue('tps' in stats_python)
        self.assertTrue('now' in stats_python)

    def test_props(self):
        vec_source = VectorSource(numpy.uint32, [0, 9, 8, 7, 6])
        vec_sink = VectorSink(numpy.uint32)
        block = MyBlock()
        self.tb.connect(vec_source, block, vec_sink)
        self.tb.run()

        blocks_json = self.tb.query("<args><path>/blocks.json</path></args>")
        print blocks_json
        blocks_python = json.loads(blocks_json)
        print blocks_python
        self.assertEqual(len(blocks_python['blocks']), 3)

if __name__ == '__main__':
    unittest.main()
