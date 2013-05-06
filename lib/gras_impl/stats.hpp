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
        init_time = time_now();
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

    time_ticks_t init_time;
    time_ticks_t start_time;
    time_ticks_t stop_time;

    //overall tracking of ports
    std::vector<item_index_t> items_consumed;
    std::vector<item_index_t> tags_consumed;
    std::vector<item_index_t> msgs_consumed;
    std::vector<item_index_t> items_produced;
    std::vector<item_index_t> tags_produced;
    std::vector<item_index_t> msgs_produced;
    std::vector<item_index_t> bytes_copied;

    //port starvation tracking
    std::vector<time_ticks_t> inputs_idle;
    std::vector<time_ticks_t> outputs_idle;

    //instantaneous port status
    size_t actor_queue_depth;
    std::vector<size_t> items_enqueued;
    std::vector<size_t> msgs_enqueued;
    std::vector<size_t> tags_enqueued;

    item_index_t work_count;
    time_ticks_t time_last_work;
    time_ticks_t total_time_prep;
    time_ticks_t total_time_work;
    time_ticks_t total_time_post;
    time_ticks_t total_time_input;
    time_ticks_t total_time_output;
};

} //namespace gras

#endif /*INCLUDED_LIBGRAS_IMPL_STATS_HPP*/
