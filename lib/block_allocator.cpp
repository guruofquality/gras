// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "element_impl.hpp"
#include <gras_impl/block_actor.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

using namespace gras;

const size_t AT_LEAST_DEFAULT_ITEMS = 1 << 13;
const size_t AHH_TOO_MANY_BYTES = 1 << 20; //TODO
const size_t THIS_MANY_BUFFERS = 32;

//TODO will need more complicated later

void BlockActor::buffer_returner(const size_t index, SBuffer &buffer)
{
    //reset offset and length
    buffer.offset = 0;
    buffer.length = 0;

    OutputBufferMessage message;
    message.index = index;
    message.buffer = buffer;
    this->Push(message, Theron::Address());
}

static size_t recommend_length(
    const std::vector<OutputHintMessage> &hints,
    const size_t hint_bytes,
    const size_t at_least_bytes,
    const size_t at_most_bytes
){
    //step 1) find the min of all reserves to create a super-reserve
    size_t min_bytes = at_least_bytes;
    BOOST_FOREACH(const OutputHintMessage &hint, hints)
    {
        min_bytes = std::max(min_bytes, hint.reserve_bytes);
    }

    //step 2) N x super reserve of hard-coded mimimum items
    size_t Nmin_bytes = min_bytes;
    while (hint_bytes > Nmin_bytes)
    {
        Nmin_bytes += min_bytes;
    }

    //step 3) cap the maximum size by the upper bound (if set)
    if (at_most_bytes) Nmin_bytes = std::min(Nmin_bytes, at_most_bytes);
    else Nmin_bytes = std::min(Nmin_bytes, AHH_TOO_MANY_BYTES);

    return Nmin_bytes;
}

void BlockActor::handle_top_alloc(const TopAllocMessage &, const Theron::Address from)
{
    MESSAGE_TRACER();

    //allocate output buffers which will also wake up the task
    const size_t num_outputs = this->get_num_outputs();
    for (size_t i = 0; i < num_outputs; i++)
    {
        const size_t bytes = recommend_length(
            this->output_allocation_hints[i],
            AT_LEAST_DEFAULT_ITEMS*this->output_items_sizes[i],
            this->output_configs[i].reserve_items*this->output_items_sizes[i],
            this->output_configs[i].maximum_items*this->output_items_sizes[i]
        );

        SBufferDeleter deleter = boost::bind(&BlockActor::buffer_returner, this, i, _1);
        SBufferToken token = SBufferToken(new SBufferDeleter(deleter));

        BufferQueueSptr queue = block_ptr->output_buffer_allocator(i, token, bytes);
        this->output_queues.set_buffer_queue(i, queue);

        InputAllocMessage message;
        message.token = SBufferToken(new SBufferDeleter(deleter));
        message.recommend_length = bytes;
        this->post_downstream(i, message);
    }

    this->Send(0, from); //ACK
}

BufferQueueSptr Block::output_buffer_allocator(
    const size_t,
    const SBufferToken &token,
    const size_t recommend_length
){
    SBufferConfig config;
    config.memory = NULL;
    config.length = recommend_length;
    config.affinity = (*this)->block->buffer_affinity;
    config.token = token;
    return BufferQueue::make_circ(config, THIS_MANY_BUFFERS);
}

BufferQueueSptr Block::input_buffer_allocator(
    const size_t,
    const SBufferToken &,
    const size_t
){
    return BufferQueueSptr(); //null
}
