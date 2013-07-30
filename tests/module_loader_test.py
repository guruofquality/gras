# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras

class ModuleLoaderTest(unittest.TestCase):

    def test_load_module_cpp(self):
        my_block = gras.make("/tests/my_block0")
        self.assertEqual(my_block.get_num(), 42)

    def test_load_module_py(self):
        my_block = gras.make("/tests/my_block1")
        self.assertEqual(my_block.get_num(), 42)

if __name__ == '__main__':
    unittest.main()
