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
        this->register_getter("foo", &MyBlock::get_foo);
        this->register_setter("foo", &MyBlock::set_foo);
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

    double my_foo_d; my_block.get("foo", my_foo_d);
    BOOST_CHECK_EQUAL(my_foo_d, double(42));
}

BOOST_AUTO_TEST_CASE(test_property_errors)
{
    MyBlock my_block;

    //property does not exist
    BOOST_CHECK_THROW(my_block.get<size_t>("bar"), std::exception);

    //wrong type for property
    BOOST_CHECK_THROW(my_block.set("foo", "a string"), std::exception);
}

struct MyBlockNewShit : gras::Block
{
    MyBlockNewShit(void):
        gras::Block("MyBlockNewShit")
    {
        this->register_call("foo", &MyBlockNewShit::foo);
        this->register_call("bar", &MyBlockNewShit::bar);
        this->register_call("baz", &MyBlockNewShit::baz);
        //this->register_call("example_setter", &MyBlockNewShit::example_setter);
    }

    //dummy work
    void work(const InputItems &, const OutputItems &){}

    int foo(void)
    {
        return 42;
    }

    int bar(const std::string &a0)
    {
        return a0.size();
    }

    int baz(const std::string &a0, const bool &a1)
    {
        return a1?a0.size():-1;
    }

    void example_setter(const long &)
    {
        
    }
};

BOOST_AUTO_TEST_CASE(test_property_newshit)
{
    MyBlockNewShit my_block;
    size_t my_foo = my_block.call<size_t>("foo");
    BOOST_CHECK_EQUAL(my_foo, size_t(42));

    size_t my_bar = my_block.call<size_t>("bar", "hello");
    BOOST_CHECK_EQUAL(my_bar, size_t(5));

    signed my_baz = my_block.call<signed>("baz", "hello", false);
    BOOST_CHECK_EQUAL(my_baz, signed(-1));

    //my_block.call<int>("example_setter", 123);
}
