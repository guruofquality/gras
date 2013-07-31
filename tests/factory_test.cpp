// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <boost/test/unit_test.hpp>
#include <iostream>

#include <gras/block.hpp>

struct MyBlock : gras::Block
{
    MyBlock(const long &a0 = 0, const std::string &a1 = "", const bool &a2 = false):
        gras::Block("MyBlock"), a0(a0), a1(a1), a2(a2)
    {
    }

    const long a0;
    const std::string a1;
    const bool a2;

    //dummy work
    void work(const InputItems &, const OutputItems &){}
};

// a few test factory functions

gras::Block *make_my_block_args0(void)
{
    return new MyBlock();
}

gras::Block *make_my_block_args1(const long &a0)
{
    return new MyBlock(a0);
}

gras::Block *make_my_block_args2(const long &a0, const std::string &a1)
{
    return new MyBlock(a0, a1);
}

gras::Block *make_my_block_args3(const long &a0, const std::string &a1, const bool &a2)
{
    return new MyBlock(a0, a1, a2);
}

#include <gras/factory.hpp>

GRAS_REGISTER_FACTORY("/tests/my_block_args0", make_my_block_args0)
GRAS_REGISTER_FACTORY("/tests/my_block_args1", make_my_block_args1)
GRAS_REGISTER_FACTORY("/tests/my_block_args2", make_my_block_args2)
GRAS_REGISTER_FACTORY("/tests/my_block_args3", make_my_block_args3)

BOOST_AUTO_TEST_CASE(test_register_and_make)
{
    gras::Element *my_block0 = gras::make("/tests/my_block_args0");
    BOOST_CHECK(dynamic_cast<MyBlock *>(my_block0) != NULL);
    BOOST_CHECK_EQUAL(dynamic_cast<MyBlock *>(my_block0)->a0, 0);
    BOOST_CHECK_EQUAL(dynamic_cast<MyBlock *>(my_block0)->a1, "");
    BOOST_CHECK_EQUAL(dynamic_cast<MyBlock *>(my_block0)->a2, false);

    gras::Element *my_block1 = gras::make("/tests/my_block_args1", 42);
    BOOST_CHECK(dynamic_cast<MyBlock *>(my_block1) != NULL);
    BOOST_CHECK_EQUAL(dynamic_cast<MyBlock *>(my_block1)->a0, 42);
    BOOST_CHECK_EQUAL(dynamic_cast<MyBlock *>(my_block1)->a1, "");
    BOOST_CHECK_EQUAL(dynamic_cast<MyBlock *>(my_block1)->a2, false);

    gras::Element *my_block2 = gras::make("/tests/my_block_args2", 1, "foo");
    BOOST_CHECK(dynamic_cast<MyBlock *>(my_block2) != NULL);
    BOOST_CHECK_EQUAL(dynamic_cast<MyBlock *>(my_block2)->a0, 1);
    BOOST_CHECK_EQUAL(dynamic_cast<MyBlock *>(my_block2)->a1, "foo");
    BOOST_CHECK_EQUAL(dynamic_cast<MyBlock *>(my_block2)->a2, false);

    gras::Element *my_block3 = gras::make("/tests/my_block_args3", -2, "bar", true);
    BOOST_CHECK(dynamic_cast<MyBlock *>(my_block3) != NULL);
    BOOST_CHECK_EQUAL(dynamic_cast<MyBlock *>(my_block3)->a0, -2);
    BOOST_CHECK_EQUAL(dynamic_cast<MyBlock *>(my_block3)->a1, "bar");
    BOOST_CHECK_EQUAL(dynamic_cast<MyBlock *>(my_block3)->a2, true);
}
