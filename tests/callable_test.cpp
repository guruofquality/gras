// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <boost/test/unit_test.hpp>
#include <iostream>

#include <gras/callable.hpp>


struct MyClass : gras::Callable
{
    MyClass(void)
    {
        count = 0;
        this->register_call("test_args0_with_return", &MyClass::test_args0_with_return);
        this->register_call("test_args1_with_return", &MyClass::test_args1_with_return);
        this->register_call("test_args2_with_return", &MyClass::test_args2_with_return);
        this->register_call("test_args3_with_return", &MyClass::test_args3_with_return);
        this->register_call("test_args0_with_no_return", &MyClass::test_args0_with_no_return);
        this->register_call("test_args1_with_no_return", &MyClass::test_args1_with_no_return);
        this->register_call("test_args2_with_no_return", &MyClass::test_args2_with_no_return);
        this->register_call("test_args3_with_no_return", &MyClass::test_args3_with_no_return);
    }

    int test_args0_with_return(void)
    {
        return count;
    }

    int test_args1_with_return(const int &a0)
    {
        return a0;
    }

    int test_args2_with_return(const int &a0, const std::string &a1)
    {
        return a0 + a1.size();
    }

    int test_args3_with_return(const int &a0, const std::string &a1, const bool &a3)
    {
        return a3? a0 + a1.size() : 0;
    }

    void test_args0_with_no_return(void)
    {
        count++;
    }

    void test_args1_with_no_return(const int &a0)
    {
        count = a0;
    }

    void test_args2_with_no_return(const int &a0, const std::string &a1)
    {
        count = a0 + a1.size();
    }

    void test_args3_with_no_return(const int &a0, const std::string &a1, const bool &a2)
    {
        if (a2) count = a0 + a1.size();
        else count = 0;
    }

    int count;
};

BOOST_AUTO_TEST_CASE(test_registered_methods)
{
    MyClass my_class;
    {
        size_t ret = my_class.x<size_t>("test_args0_with_return");
        BOOST_CHECK_EQUAL(ret, size_t(0));
    }
    {
        size_t ret = my_class.x<size_t>("test_args1_with_return", 42);
        BOOST_CHECK_EQUAL(ret, size_t(42));
    }
    {
        size_t ret = my_class.x<size_t>("test_args2_with_return", 1, "OK");
        BOOST_CHECK_EQUAL(ret, size_t(3));
    }
    {
        size_t ret = my_class.x<size_t>("test_args3_with_return", 1, "OK", true);
        BOOST_CHECK_EQUAL(ret, size_t(3));
    }
    {
        size_t ret = my_class.x<size_t>("test_args3_with_return", 1, "OK", false);
        BOOST_CHECK_EQUAL(ret, size_t(0));
    }
    {
        my_class.x("test_args0_with_no_return"); //this adds 1 to count
        size_t ret = my_class.x<size_t>("test_args0_with_return");
        BOOST_CHECK_EQUAL(ret, size_t(1));
    }
    {
        my_class.x("test_args1_with_no_return", 42);
        size_t ret = my_class.x<size_t>("test_args0_with_return");
        BOOST_CHECK_EQUAL(ret, size_t(42));
    }
    {
        my_class.x("test_args2_with_no_return", 1, "OK");
        size_t ret = my_class.x<size_t>("test_args0_with_return");
        BOOST_CHECK_EQUAL(ret, size_t(3));
    }
    {
        my_class.x("test_args3_with_no_return", 1, "OK", true);
        size_t ret = my_class.x<size_t>("test_args0_with_return");
        BOOST_CHECK_EQUAL(ret, size_t(3));
    }
    {
        my_class.x("test_args3_with_no_return", 1, "OK", false);
        size_t ret = my_class.x<size_t>("test_args0_with_return");
        BOOST_CHECK_EQUAL(ret, size_t(0));
    }
}
