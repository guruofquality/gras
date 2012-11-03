
import unittest
import gras
import numpy

class BlockTest(unittest.TestCase):

    def test_make_block(self):
        b = gras.Block('FooBLock')
        b.set_output_signature([numpy.int32])

if __name__ == '__main__':
    unittest.main()
