# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras

class HierBlockTest(unittest.TestCase):

    def test_expected_attrs(self):
        self.assertTrue(hasattr(gras, 'HierBlock'))
        self.assertTrue(hasattr(gras, 'TopBlock'))

    def test_make_hier(self):
        h = gras.HierBlock('my_hb')
        print h

    def test_make_top(self):
        t = gras.TopBlock('my_tb')
        print t

if __name__ == '__main__':
    unittest.main()
