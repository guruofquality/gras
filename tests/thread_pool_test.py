# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
from gras import TestUtils
import numpy

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
        vec_source = TestUtils.VectorSource(numpy.uint32, [0, 9, 8, 7, 6])
        vec_sink = TestUtils.VectorSink(numpy.uint32)

        c = gras.ThreadPoolConfig()
        tp = gras.ThreadPool(c)

        vec_source.global_config().thread_pool = tp
        vec_source.commit_config()
        tb.connect(vec_source, vec_sink)
        tb.run()

        self.assertEqual(vec_sink.data(), (0, 9, 8, 7, 6))

if __name__ == '__main__':
    unittest.main()
