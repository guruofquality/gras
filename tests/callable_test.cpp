// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <boost/test/unit_test.hpp>
#include <iostream>

#include <gras/callable.hpp>


struct MyClass : gras::Callable
{
    MyClass(void)
    {
        this->register_call("foo", &MyClass::foo);
        this->register_call("bar", &MyClass::bar);
        this->register_call("baz", &MyClass::baz);
        //this->register_call("example_setter", &MyClass::example_setter);
    }

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
    MyClass my_class;
    size_t my_foo = my_class.call<size_t>("foo");
    BOOST_CHECK_EQUAL(my_foo, size_t(42));

    size_t my_bar = my_class.call<size_t>("bar", "hello");
    BOOST_CHECK_EQUAL(my_bar, size_t(5));

    signed my_baz = my_class.call<signed>("baz", "hello", false);
    BOOST_CHECK_EQUAL(my_baz, signed(-1));

    //my_class.call("example_setter", 123);
}
