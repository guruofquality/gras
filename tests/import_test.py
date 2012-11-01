
import unittest

class ImportTest(unittest.TestCase):

    def test_import(self):
        import gras

    def test_expected_attrs(self):
        import gras
        self.assertTrue(hasattr(gras, 'HierBlock'))
        self.assertTrue(hasattr(gras, 'TopBlock'))

    def test_make_hier(self):
        import gras
        h = gras.HierBlock('my_hb')
        print h

    def test_make_top(self):
        import gras
        t = gras.TopBlock('my_tb')
        print t

if __name__ == '__main__':
    unittest.main()
