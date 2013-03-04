// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_LIBGRAS_IMPL_STATS_HPP
#define INCLUDED_LIBGRAS_IMPL_STATS_HPP

#include <gras/chrono.hpp>
#include <vector>

namespace gras
{

struct BlockStats
{
    BlockStats(void)
    {
        start_time = 0;
        stop_time = 0;
        work_count = 0;
        time_last_work = 0;
        total_time_prep = 0;
        total_time_work = 0;
        total_time_post = 0;
        total_time_input = 0;
        total_time_output = 0;
    }

    time_ticks_t start_time;
    time_ticks_t stop_time;

    std::vector<item_index_t> items_consumed;
    std::vector<item_index_t> tags_consumed;
    std::vector<item_index_t> items_produced;
    std::vector<item_index_t> tags_produced;

    item_index_t work_count;
    time_ticks_t time_last_work;
    time_ticks_t total_time_prep;
    time_ticks_t total_time_work;
    time_ticks_t total_time_post;
    time_ticks_t total_time_input;
    time_ticks_t total_time_output;
};

struct TimerAccumulate
{
    TimerAccumulate(time_ticks_t &accum):
        accum(accum), t0(time_now()){}
    ~TimerAccumulate(void)
    {
        if (t0) this->done();
    }
    void done(void)
    {
        accum += (time_now()-t0);
        t0 = 0;
    }
    time_ticks_t &accum;
    time_ticks_t t0;
};

} //namespace gras

#endif /*INCLUDED_LIBGRAS_IMPL_STATS_HPP*/
