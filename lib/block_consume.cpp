// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "element_impl.hpp"
#include <gras_impl/block_actor.hpp>
#include <gras/block.hpp>

using namespace gras;

void Block::consume(const size_t which_input, const size_t num_items)
{
    ASSERT(long(num_items) >= 0); //sign bit set? you dont want a negative
    (*this)->block_actor->consume(which_input, num_items);
}

void Block::consume(const size_t num_items)
{
    const size_t num_inputs = (*this)->worker->get_num_inputs();
    for (size_t i = 0; i < num_inputs; i++)
    {
        (*this)->block_actor->consume(i, num_items);
    }
}

item_index_t Block::get_consumed(const size_t which_input)
{
    return (*this)->block_data->total_items_consumed[which_input];
}

SBuffer Block::get_input_buffer(const size_t which_input) const
{
    return (*this)->block_data->input_queues.front(which_input);
}

GRAS_FORCE_INLINE void BlockActor::consume(const size_t i, const size_t items)
{
    #ifdef ITEM_CONSPROD
    std::cerr << name << " consume " << items << std::endl;
    #endif
    data->stats.items_consumed[i] += items;
    data->num_input_items_read[i] += items;
}
