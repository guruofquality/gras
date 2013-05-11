# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras

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

if __name__ == '__main__':
    unittest.main()
