# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras

class SBufferTest(unittest.TestCase):

    def test_make_config(self):
        c = gras.SBufferConfig()
        print c.length
        print c.memory

    def test_make_sbuffer(self):
        c = gras.SBufferConfig()
        c.length = 42
        s = gras.SBuffer(c)
        self.assertEqual(c.length, s.get_actual_length())
        self.assertEqual(c.length, s.length)
        self.assertEqual(c.length, len(s))

    def test_get_sbuffer(self):
        c = gras.SBufferConfig()
        c.length = 24
        s = gras.SBuffer(c)
        a = s.get()
        self.assertEqual(c.length, len(s))
        self.assertEqual(c.length, len(a))

    def test_more_get_with_offset(self):
        c = gras.SBufferConfig()
        c.length = 4
        s = gras.SBuffer(c)
        a0 = s.get()
        a0[:] = [1, 2, 3, 4]
        a1 = s.get()
        self.assertEqual(tuple(a1), (1, 2, 3, 4))

        #now offset the start of buffer
        s.offset = 2
        s.length = 2
        a2 = s.get()
        self.assertEqual(2, len(s))
        self.assertEqual(2, len(a2))
        self.assertEqual(tuple(a2), (3, 4))

if __name__ == '__main__':
    unittest.main()
