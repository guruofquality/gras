// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <boost/test/unit_test.hpp>
#include <iostream>

#include <gras/chrono.hpp>
#include <boost/thread/thread.hpp> //sleep

BOOST_AUTO_TEST_CASE(test_chrono_time)
{
    for (size_t trial_i = 0; trial_i < 100; trial_i++)
    {
        const double sleep_secs = 0.01 + trial_i*0.0001;
        gras::time_ticks_t t0 = gras::time_now();
        boost::this_thread::sleep(boost::posix_time::milliseconds(long(sleep_secs*1000)));
        gras::time_ticks_t t1 = gras::time_now();

        std::cout << "t0 " << t0 << std::endl;
        std::cout << "t1 " << t1 << std::endl;
        BOOST_CHECK(t1 > t0);

        const double delta_time = double(t1-t0)/gras::time_tps();
        std::cout << "delta_time " << delta_time << std::endl;
        const double low_margin = sleep_secs - sleep_secs/2;
        const double high_margin = sleep_secs + sleep_secs/2;
        BOOST_CHECK(delta_time < high_margin and delta_time > low_margin);
    }
}
