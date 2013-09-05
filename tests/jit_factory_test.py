# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import numpy
import time
from gras import TestUtils

#figure out the local include directories
import os
root_dir = os.path.join(os.path.dirname(__file__), '..')
gras_inc = os.path.join(root_dir, 'include')
pmc_inc = os.path.join(root_dir, 'PMC', 'include')

ADD_F32_SOURCE = """
#include <gras/block.hpp>
#include <gras/factory.hpp>
#include <iostream>

struct MyAddFloat32 : gras::Block
{
    MyAddFloat32(void):
        gras::Block("MyAddFloat32")
    {
        this->input_config(0).item_size = sizeof(float);
        this->output_config(0).item_size = sizeof(float);
    }

    void work(const InputItems &ins, const OutputItems &outs)
    {
        const size_t n_nums = std::min(ins.min(), outs.min());
        float *out = outs[0].cast<float *>();
        const float *in0 = ins[0].cast<const float *>();
        const float *in1 = ins[1].cast<const float *>();

        for (size_t i = 0; i < n_nums; i++)
        {
            out[i] = in0[i] + in1[i];
        }

        this->consume(n_nums);
        this->produce(n_nums);
    }
};

GRAS_REGISTER_FACTORY0("/tests/my_add_f32", MyAddFloat32)
"""

ADD_CONST_F32_SOURCE = """
#include <gras/block.hpp>
#include <gras/factory.hpp>
#include <iostream>

struct MyAddConstFloat32 : gras::Block
{
    MyAddConstFloat32(void):
        gras::Block("MyAddConstFloat32")
    {
        this->input_config(0).item_size = sizeof(float);
        this->output_config(0).item_size = sizeof(float);
        this->set_value(0); //initial state
        this->register_call("set_value", &MyAddConstFloat32::set_value);
        this->register_call("get_value", &MyAddConstFloat32::get_value);
    }

    void set_value(const float &value)
    {
        _value = value;
    }

    float get_value(void)
    {
        return _value;
    }

    void work(const InputItems &ins, const OutputItems &outs)
    {
        const size_t n_nums = std::min(ins.min(), outs.min());
        float *out = outs[0].cast<float *>();
        const float *in = ins[0].cast<const float *>();

        for (size_t i = 0; i < n_nums; i++)
        {
            out[i] = in[i] + _value;
        }

        this->consume(n_nums);
        this->produce(n_nums);
    }

    float _value;
};

GRAS_REGISTER_FACTORY0("/tests/my_add_const_f32", MyAddConstFloat32)
"""

class JITFactoryTest(unittest.TestCase):

    def setUp(self):
        self.tb = gras.TopBlock()

    def tearDown(self):
        self.tb = None

    def test_add_float32(self):

        gras.jit_factory(ADD_F32_SOURCE, ["-O3", "-I"+gras_inc, "-I"+pmc_inc])
        op = gras.make("/tests/my_add_f32")

        vec0 = numpy.array(numpy.random.randint(-150, +150, 10000), numpy.float32)
        vec1 = numpy.array(numpy.random.randint(-150, +150, 10000), numpy.float32)

        src0 = TestUtils.VectorSource(numpy.float32, vec0)
        src1 = TestUtils.VectorSource(numpy.float32, vec1)
        dst = TestUtils.VectorSink(numpy.float32)

        self.tb.connect(src0, (op, 0))
        self.tb.connect(src1, (op, 1))
        self.tb.connect(op, dst)
        self.tb.run()

        expected_result = list(vec0 + vec1)
        actual_result = list(dst.data())

        self.assertEqual(expected_result, actual_result)

    def test_add_const_float32(self):

        gras.jit_factory(ADD_CONST_F32_SOURCE, ["-O3", "-I"+gras_inc, "-I"+pmc_inc])
        op = gras.make("/tests/my_add_const_f32")

        offset = 42.
        op.set_value(offset) #set offset for test
        self.assertAlmostEqual(op.get_value(), offset)

        vec = numpy.array(numpy.random.randint(-150, +150, 10000), numpy.float32)

        src = TestUtils.VectorSource(numpy.float32, vec)
        dst = TestUtils.VectorSink(numpy.float32)

        self.tb.connect(src, op, dst)
        self.tb.run()

        expected_result = list(vec + offset)
        actual_result = list(dst.data())

        self.assertEqual(expected_result, actual_result)

    def test_jit_compile_special(self):
        SOURCE = """
#include <gras/block.hpp>
#include <gras/factory.hpp>
#include <iostream>

struct FooBar : gras::Block
{
    FooBar(void):
        gras::Block("FooBar")
    {
    }

    void work(const InputItems &ins, const OutputItems &outs)
    {
        gras::SBufferConfig c0;
        gras::SBuffer b0 = gras::SBuffer(c0);
        gras::SBuffer i0 = this->get_input_buffer(0);
    }
};

GRAS_REGISTER_FACTORY0("/tests/my_foo_bar", FooBar)
"""
        gras.jit_factory(SOURCE, ["-O3", "-I"+gras_inc, "-I"+pmc_inc])

if __name__ == '__main__':
    unittest.main()
