# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras

class IOSigTest(unittest.TestCase):

    def test_io_sig(self):
        io = gras.IOSignature()
        self.assertEqual(len(io), 0)

if __name__ == '__main__':
    unittest.main()
