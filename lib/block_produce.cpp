// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "element_impl.hpp"
#include <gras_impl/block_actor.hpp>
#include <gras/block.hpp>

using namespace gras;

void Block::produce(const size_t which_output, const size_t num_items)
{
    ASSERT(long(num_items) >= 0); //sign bit set? you dont want a negative
    (*this)->block->produce(which_output, num_items);
}

void Block::produce(const size_t num_items)
{
    const size_t num_outputs = (*this)->block->get_num_outputs();
    for (size_t o = 0; o < num_outputs; o++)
    {
        (*this)->block->produce(o, num_items);
    }
}

item_index_t Block::get_produced(const size_t which_output)
{
    return (*this)->block_data->stats.items_produced[which_output];
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

void Block::post_output_buffer(const size_t which_output, const SBuffer &buffer)
{
    (*this)->block->produce_buffer(which_output, buffer);
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
    data->produce_outputs[i] = true;
}

GRAS_FORCE_INLINE void BlockActor::produce_buffer(const size_t i, const SBuffer &buffer)
{
    data->output_queues.consume(i);
    ASSERT((buffer.length % data->output_configs[i].item_size) == 0);
    const size_t items = buffer.length/data->output_configs[i].item_size;
    data->stats.items_produced[i] += items;
    InputBufferMessage buff_msg;
    buff_msg.buffer = buffer;
    this->post_downstream(i, buff_msg);
}
