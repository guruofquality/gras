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
        this->register_call("get_foo", &MyBlock::get_foo);
        this->register_call("set_foo", &MyBlock::set_foo);
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

BOOST_AUTO_TEST_CASE(test_calls_get_set)
{
    MyBlock my_block;
    BOOST_CHECK_EQUAL(my_block.foo, size_t(0));

    my_block.x("set_foo", size_t(42));
    BOOST_CHECK_EQUAL(my_block.foo, size_t(42));

    const size_t my_foo = my_block.x<size_t>("get_foo");
    BOOST_CHECK_EQUAL(my_foo, size_t(42));
}

BOOST_AUTO_TEST_CASE(test_callable_errors)
{
    MyBlock my_block;

    //call does not exist
    BOOST_CHECK_THROW(my_block.x<size_t>("get_bar"), std::exception);

    //wrong type for call
    BOOST_CHECK_THROW(my_block.x("set_foo", "a string"), std::exception);
}
