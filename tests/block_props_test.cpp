// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <boost/test/unit_test.hpp>
#include <iostream>

#include <gras/block.hpp>

struct MyBlock : gras::Block
{
    MyBlock(void):
        gras::Block("MyBlock")
    {
        foo = 0;
        this->register_property("foo", &MyBlock::get_foo, &MyBlock::set_foo);
    }

    //dummy work
    void work(const InputItems &, const OutputItems &){}

    size_t get_foo(void)
    {
        return foo;
    }

    void set_foo(const size_t &new_foo)
    {
        foo = new_foo;
    }

    size_t foo;
};

BOOST_AUTO_TEST_CASE(test_property_set_get_with_return)
{
    MyBlock my_block;
    BOOST_CHECK_EQUAL(my_block.foo, size_t(0));

    my_block.set("foo", size_t(42));
    BOOST_CHECK_EQUAL(my_block.foo, size_t(42));

    const size_t my_foo = my_block.get<size_t>("foo");
    BOOST_CHECK_EQUAL(my_foo, size_t(42));
}

BOOST_AUTO_TEST_CASE(test_property_set_get_with_reference)
{
    MyBlock my_block;
    BOOST_CHECK_EQUAL(my_block.foo, size_t(0));

    my_block.set("foo", size_t(42));
    BOOST_CHECK_EQUAL(my_block.foo, size_t(42));

    size_t my_foo; my_block.get("foo", my_foo);
    BOOST_CHECK_EQUAL(my_foo, size_t(42));
}

BOOST_AUTO_TEST_CASE(test_property_errors)
{
    MyBlock my_block;

    //property does not exist
    BOOST_CHECK_THROW(my_block.get<size_t>("bar"), std::exception);

    //wrong type for property
    BOOST_CHECK_THROW(my_block.set("foo", double(42)), std::exception);
}
