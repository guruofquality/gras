# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import gras

class MyBlock(gras.Block):
    def __init__(self):
        gras.Block.__init__(self, "MyBlock")
        self.foo = 0
        self.register_call("get_num", self.get_num)

    def work(self, *args): pass

    def get_num(self):
        return 42

gras.register_factory("/tests/my_block1", MyBlock)
