// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_LIBGRAS_IMPL_STATS_HPP
#define INCLUDED_LIBGRAS_IMPL_STATS_HPP

#include <gras/chrono.hpp>
#include <vector>

namespace gras
{

struct BlockStats
{
    time_ticks_t start_time;
    time_ticks_t stop_time;

    std::vector<item_index_t> items_consumed;
    std::vector<item_index_t> tags_consumed;
    std::vector<item_index_t> items_produced;
    std::vector<item_index_t> tags_produced;

    item_index_t work_count;
    time_ticks_t time_last_work;
    time_ticks_t total_time_work;
    time_ticks_t total_time_work_other;
};

} //namespace gras

#endif /*INCLUDED_LIBGRAS_IMPL_STATS_HPP*/
