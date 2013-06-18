# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
from PMC import *
import numpy

class SBufferTest(unittest.TestCase):

    def test_make_config(self):
        c = gras.SBufferConfig()
        print c.length
        print c.memory

    def test_null_sbuffer(self):
        s = gras.SBuffer()
        self.assertFalse(s)

    def test_make_sbuffer(self):
        c = gras.SBufferConfig()
        c.length = 42
        s = gras.SBuffer(c)
        self.assertTrue(s)
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

    def test_get_zero_len_sbuffer(self):
        c = gras.SBufferConfig()
        c.length = 0
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

    def test_with_pmc(self):
        c = gras.SBufferConfig()
        c.length = 4
        s0 = gras.SBuffer(c)
        p = PMC_M(s0)
        print p
        s1 = p()
        self.assertEqual(c.length, s1.length)
        self.assertEqual(c.length, len(s1))

        s0.get()[:] = numpy.random.randint(0, 200, 4)
        self.assertEqual(tuple(s0.get()), tuple(s1.get()))

if __name__ == '__main__':
    unittest.main()
