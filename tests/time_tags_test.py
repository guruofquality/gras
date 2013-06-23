# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import numpy
import time

class TimeTagsTest(unittest.TestCase):

    def test_make_time_tag(self):
        t0 = gras.TimeTag()

    def test_make_time_tag_from_ticks(self):
        t0 = gras.TimeTag.from_ticks(42)
        self.assertEqual(t0.to_ticks(), 42)

        t1 = gras.TimeTag.from_ticks(1000, 1e9)
        self.assertEqual(t1.to_ticks(1e9), 1000)

        t2 = gras.TimeTag.from_ticks(-1000, 1e9)
        self.assertEqual(t2.to_ticks(1e9), -1000)

        time_ns_now = long(time.time()*1e9)
        t3 = gras.TimeTag.from_ticks(time_ns_now, 1e9)
        self.assertEqual(t3.to_ticks(1e9), time_ns_now)

        time_us_now = long(time.time()*1e6)
        t4 = gras.TimeTag.from_ticks(time_us_now, 1e6)
        self.assertEqual(t4.to_ticks(1e6), time_us_now)

    def test_time_tag_compare(self):
        t0 = gras.TimeTag.from_ticks(42)
        t1 = gras.TimeTag.from_ticks(-1000)
        self.assertEqual(t0, t0)
        self.assertEqual(t1, t1)
        self.assertGreater(t0, t1)
        self.assertLess(t1, t0)

    def test_time_tag_add(self):
        t0 = gras.TimeTag.from_ticks(42)
        t1 = gras.TimeTag.from_ticks(-1000)
        t2 = t0 + t1
        self.assertEqual(t2.to_ticks(), (42 + -1000))

        t3 = gras.TimeTag.from_ticks(99)
        t4 = gras.TimeTag.from_ticks(-98)
        t3 += t4
        self.assertEqual(t3.to_ticks(), (99 + -98))

    def test_time_tag_sub(self):
        t0 = gras.TimeTag.from_ticks(42)
        t1 = gras.TimeTag.from_ticks(-1000)
        t2 = t0 - t1
        self.assertEqual(t2.to_ticks(), (42 + 1000))

        t3 = gras.TimeTag.from_ticks(99)
        t4 = gras.TimeTag.from_ticks(-98)
        t3 -= t4
        self.assertEqual(t3.to_ticks(), (99 + 98))

    def test_time_tag_pmc_foo(self):
        time_us_now = long(time.time()*1e6)
        t0 = gras.TimeTag.from_ticks(time_us_now, 1e6)

        #test the validity of the time tuple
        p0 = t0.to_pmc()
        tp = p0()
        full = long(time_us_now/1e6)
        self.assertEqual(tp[0], full)
        delta = time_us_now - full*long(1e6)
        self.assertEqual(tp[1], (delta/1e6))

        #and check if it loops back
        t1 = gras.TimeTag.from_pmc(p0)
        self.assertEqual(t0, t1)

if __name__ == '__main__':
    unittest.main()
