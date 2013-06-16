// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "element_impl.hpp"
#include <gras_impl/block_actor.hpp>

using namespace gras;

void Block::mark_done(void)
{
    (*this)->block_actor->mark_done();
}

void BlockActor::mark_done(void)
{
    if (data->block_state == BLOCK_STATE_DONE) return; //can re-enter checking done first

    data->stats.stop_time = time_now();
    data->block->notify_inactive();

    //flush partial output buffers to the downstream
    for (size_t i = 0; i < worker->get_num_outputs(); i++)
    {
        if (not data->output_queues.ready(i)) continue;
        SBuffer &buff = data->output_queues.front(i);
        if (buff.length == 0) continue;
        InputBufferMessage buff_msg;
        buff_msg.buffer = buff;
        worker->post_downstream(i, buff_msg);
        data->output_queues.pop(i);
    }

    data->interruptible_thread.reset();

    //mark down the new state
    data->block_state = BLOCK_STATE_DONE;

    //release upstream, downstream, and executor tokens
    data->token_pool.clear();

    //release all buffers in queues
    data->input_queues.flush_all();
    data->output_queues.flush_all();

    //release all tags and msgs
    for (size_t i = 0; i < worker->get_num_inputs(); i++)
    {
        data->input_msgs[i].clear();
        data->input_tags[i].clear();
        data->num_input_items_read[i] = 0;
        data->num_input_msgs_read[i] = 0;
    }

    //tell the upstream and downstram to re-check their tokens
    //this is how the other blocks know who is interested,
    //and can decide based on interest to set done or not
    for (size_t i = 0; i < worker->get_num_inputs(); i++)
    {
        worker->post_upstream(i, OutputCheckMessage());
    }
    for (size_t i = 0; i < worker->get_num_outputs(); i++)
    {
        worker->post_downstream(i, InputCheckMessage());
    }

    if (DONE_PRINTS) std::cerr
        << "==================================================\n"
        << "== The " << name << " is done...\n"
        << "==================================================\n"
        << std::flush;
}
