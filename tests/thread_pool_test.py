# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
from demo_blocks import *

class ThreadPoolTest(unittest.TestCase):

    def test_make_config(self):
        c = gras.ThreadPoolConfig()
        print c.thread_count
        print c.yield_strategy

    def test_make_tp(self):
        c = gras.ThreadPoolConfig()
        tp = gras.ThreadPool(c)

    def test_make_tp_set_active(self):
        c = gras.ThreadPoolConfig()
        tp = gras.ThreadPool(c)
        tp.set_active()

    def test_thread_priority(self):
        #here we assume prio 0.0 (default) can always be set
        self.assertTrue(gras.ThreadPool.test_thread_priority(0.0))

    def test_migrate_to_thread_pool(self):
        tb = gras.TopBlock()
        vec_source = VectorSource(numpy.uint32, [0, 9, 8, 7, 6])
        vec_sink = VectorSink(numpy.uint32)

        c = gras.ThreadPoolConfig()
        tp = gras.ThreadPool(c)

        vec_source.set_thread_pool(tp)
        tb.connect(vec_source, vec_sink)
        tb.run()

        self.assertEqual(vec_sink.get_vector(), (0, 9, 8, 7, 6))

if __name__ == '__main__':
    unittest.main()
