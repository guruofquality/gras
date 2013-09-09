// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "element_impl.hpp"
#include <gras_impl/block_actor.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

using namespace gras;

const size_t AT_LEAST_BYTES = 32*(1024); //kiB per buffer
const size_t AHH_TOO_MANY_BYTES = 32*(1024*1024); //MiB enough for me
const size_t THIS_MANY_BUFFERS = 8; //pool size

static void buffer_returner(ThreadPool tp, Theron::Address addr, const size_t index, SBuffer &buffer)
{
    //reset offset and length
    buffer.offset = 0;
    buffer.length = 0;
    buffer.last = NULL;

    OutputBufferMessage message;
    message.index = index;
    message.buffer = buffer;
    tp->Send(message, Theron::Address::Null(), addr);
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

#define my_round_up_mult(num, mult) (((num)*(mult))+(mult)-1)/(mult)

void BlockActor::handle_top_alloc(const TopAllocMessage &, const Theron::Address from)
{
    MESSAGE_TRACER();

    //allocate output buffers which will also wake up the task
    const size_t num_outputs = worker->get_num_outputs();
    for (size_t i = 0; i < num_outputs; i++)
    {
        size_t reserve_items = data->output_configs[i].reserve_items;
        size_t maximum_items = data->output_configs[i].maximum_items;
        if (maximum_items == 0) maximum_items = data->block->global_config().maximum_output_items;

        const size_t bytes = recommend_length(
            data->output_allocation_hints[i],
            my_round_up_mult(AT_LEAST_BYTES, data->output_configs[i].item_size),
            reserve_items*data->output_configs[i].item_size,
            maximum_items*data->output_configs[i].item_size
        );

        SBufferDeleter deleter = boost::bind(&buffer_returner, this->thread_pool, this->GetAddress(), i, _1);
        SBufferToken token = SBufferToken(new SBufferDeleter(deleter));

        SBufferConfig config;
        config.memory = NULL;
        config.length = bytes;
        config.affinity = data->block->global_config().buffer_affinity;
        config.token = token;

        BufferQueueSptr queue = data->block->output_buffer_allocator(i, config);
        data->output_queues.set_buffer_queue(i, queue);

        InputAllocMessage message;
        message.config = config;
        message.token = token;
        worker->post_downstream(i, message);
    }

    this->Send(0, from); //ACK
}

BufferQueueSptr Block::output_buffer_allocator(
    const size_t, const SBufferConfig &config
){
    return BufferQueue::make_pool(config, THIS_MANY_BUFFERS);
}

BufferQueueSptr Block::input_buffer_allocator(
    const size_t, const SBufferConfig &
){
    return BufferQueueSptr(); //null
}
