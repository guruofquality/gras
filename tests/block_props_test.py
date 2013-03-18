# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import numpy

class MyBlock(gras.Block):
    def __init__(self):
        gras.Block.__init__(self, "MyBlock")
        self.foo = 0
        self.register_property("foo", self.get_foo, self.set_foo)

    def work(self, *args): pass

    def get_foo(self):
        return self.foo

    def set_foo(self, new_foo):
        self.foo = new_foo

class BlockPropsTest(unittest.TestCase):

    def test_property_set_get(self):
        my_block = MyBlock()
        self.assertEqual(my_block.foo, 0)

        my_block.set("foo", 42)
        self.assertEqual(my_block.foo, 42)

        my_foo = my_block.get("foo")
        self.assertEqual(my_foo, 42)

    def test_property_errors(self):
        my_block = MyBlock()

        #property does not exist
        threw = False
        try: my_block.get("bar")
        except Exception as ex:
            print ex
            threw = True
        self.assertFalse(threw)

        #wrong type for property
        threw = False
        try: my_block.set("foo", float(42))
        except Exception as ex:
            print ex
            threw = True
        self.assertFalse(threw)

if __name__ == '__main__':
    unittest.main()
