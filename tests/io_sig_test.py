# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras

class IOSigTest(unittest.TestCase):

    def test_io_sig(self):
        io = gras.IOSignature()
        self.assertEqual(len(io), 0)

    def test_io_sig_extend(self):
        io = gras.IOSignature([5, 6, 7])
        self.assertEqual(len(io), 3)
        self.assertEqual(io[0], 5)
        self.assertEqual(io[1], 6)
        self.assertEqual(io[2], 7)
        self.assertEqual(io[3], 7) #auto extend

if __name__ == '__main__':
    unittest.main()
