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
        print "new_foo", new_foo
        new_foo + 0 #throws if its not a number
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
        except: threw = True
        self.assertTrue(threw)

        #wrong type for property
        threw = False
        try: my_block.set("foo", None)
        except: threw = True
        self.assertTrue(threw)

    def test_element_tree_paths(self):
        my_block = MyBlock()
        tb = gras.TopBlock()
        hb = gras.HierBlock()

        tb.adopt_element("my_hier", hb)
        hb.adopt_element("my_block", my_block)

        my_block.set("foo", 42)
        self.assertEqual(my_block.get("foo"), 42)

        my_block0 = tb.locate_block('/my_hier/my_block')
        self.assertEqual(my_block0.get("foo"), 42)

        my_block1 = hb.locate_block('my_block')
        self.assertEqual(my_block1.get("foo"), 42)

        my_block2 = hb.locate_block('./../my_hier/my_block')
        self.assertEqual(my_block2.get("foo"), 42)

        threw = False
        try: hb.locate_block('../../my_hier/my_block')
        except: threw = True
        self.assertTrue(threw)

        threw = False
        try: hb.locate_block('../../my_hier/my_block0')
        except: threw = True
        self.assertTrue(threw)

        threw = False
        try: hb.locate_block('../../my_hier/my_block/test')
        except: threw = True
        self.assertTrue(threw)

if __name__ == '__main__':
    unittest.main()
