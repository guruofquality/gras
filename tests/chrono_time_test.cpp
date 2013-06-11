// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <boost/test/unit_test.hpp>
#include <iostream>

#include <gras/chrono.hpp>
#include <boost/thread/thread.hpp> //sleep

BOOST_AUTO_TEST_CASE(test_chrono_time)
{
    gras::time_ticks_t t0 = gras::time_now();
    boost::this_thread::sleep(boost::posix_time::seconds(1));
    gras::time_ticks_t t1 = gras::time_now();

    std::cout << "t0 " << t0 << std::endl;
    std::cout << "t1 " << t1 << std::endl;
    BOOST_CHECK(t1 > t0);

    const double delta_time = double(t1-t0)/gras::time_tps();
    std::cout << "delta_time " << delta_time << std::endl;
    //this check allows +/- 0.5 seconds -- so it should always pass
    BOOST_CHECK(delta_time < 1.5 and delta_time > 0.5);
}
