
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

if __name__ == '__main__':
    unittest.main()
