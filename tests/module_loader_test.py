# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras

class ModuleLoaderTest(unittest.TestCase):

    def test_load_module(self):
        my_block = gras.Factory.make("/tests/my_block")
        self.assertEqual(my_block.get_num(), 42)

if __name__ == '__main__':
    unittest.main()
