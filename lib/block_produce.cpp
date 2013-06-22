// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "element_impl.hpp"
#include <gras_impl/block_actor.hpp>
#include <gras/block.hpp>

using namespace gras;

void Block::produce(const size_t which_output, const size_t num_items)
{
    ASSERT(long(num_items) >= 0); //sign bit set? you dont want a negative
    (*this)->block_actor->produce(which_output, num_items);
}

void Block::produce(const size_t num_items)
{
    const size_t num_outputs = (*this)->worker->get_num_outputs();
    for (size_t o = 0; o < num_outputs; o++)
    {
        (*this)->block_actor->produce(o, num_items);
    }
}

item_index_t Block::get_produced(const size_t which_output)
{
    return (*this)->block_data->total_items_produced[which_output];
}

SBuffer Block::get_output_buffer(const size_t which_output) const
{
    SBuffer &buff = (*this)->block_data->output_queues.front(which_output);
    //increment length to auto pop full buffer size,
    //when user doesnt call pop_output_buffer()
    buff.length = buff.get_actual_length();
    return buff;
}

void Block::pop_output_buffer(const size_t which_output, const size_t num_bytes)
{
    (*this)->block_data->output_queues.front(which_output).length = num_bytes;
}

void Block::post_output_buffer(const size_t i, const SBuffer &buffer)
{
    (*this)->block_data->output_queues.set_inline(i, buffer);
    const size_t item_size = (*this)->block_data->output_configs[i].item_size;
    ASSERT((buffer.length % item_size) == 0);
    const size_t items = buffer.length/item_size;
    this->produce(i, items);
}

GRAS_FORCE_INLINE void BlockActor::produce(const size_t i, const size_t items)
{
    #ifdef ITEM_CONSPROD
    std::cerr << name << " produce " << items << std::endl;
    #endif
    SBuffer &buff = data->output_queues.front(i);
    ASSERT((buff.length % data->output_configs[i].item_size) == 0);
    data->stats.items_produced[i] += items;
    const size_t bytes = items*data->output_configs[i].item_size;
    buff.length += bytes;
    data->num_output_items_read[i] += items;
}
