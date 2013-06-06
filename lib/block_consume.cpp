// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "element_impl.hpp"
#include <gras_impl/block_actor.hpp>
#include <gras/block.hpp>
#include "tag_handlers.hpp"

using namespace gras;

void Block::consume(const size_t which_input, const size_t num_items)
{
    ASSERT(long(num_items) >= 0); //sign bit set? you dont want a negative
    (*this)->block->consume(which_input, num_items);
}

void Block::consume(const size_t num_items)
{
    const size_t num_inputs = (*this)->block->get_num_inputs();
    for (size_t i = 0; i < num_inputs; i++)
    {
        (*this)->block->consume(i, num_items);
    }
}

item_index_t Block::get_consumed(const size_t which_input)
{
    return (*this)->block_data->stats.items_consumed[which_input];
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
    const size_t bytes = items*data->input_configs[i].item_size;
    data->input_queues.consume(i, bytes);
    this->trim_tags(i);
}
