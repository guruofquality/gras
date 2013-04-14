// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "element_impl.hpp"
#include <gras_impl/block_actor.hpp>

using namespace gras;

void Block::mark_done(void)
{
    (*this)->block->mark_done();
}

void BlockActor::mark_done(void)
{
    if (this->block_state == BLOCK_STATE_DONE) return; //can re-enter checking done first

    this->stats.stop_time = time_now();
    this->block_ptr->notify_inactive();

    //flush partial output buffers to the downstream
    for (size_t i = 0; i < this->get_num_outputs(); i++)
    {
        if (not this->output_queues.ready(i)) continue;
        SBuffer &buff = this->output_queues.front(i);
        if (buff.length == 0) continue;
        InputBufferMessage buff_msg;
        buff_msg.buffer = buff;
        this->post_downstream(i, buff_msg);
        this->output_queues.pop(i);
    }

    this->interruptible_thread.reset();

    //mark down the new state
    this->block_state = BLOCK_STATE_DONE;

    //release upstream, downstream, and executor tokens
    this->token_pool.clear();

    //release all buffers in queues
    this->input_queues.flush_all();
    this->output_queues.flush_all();

    //release all tags and msgs
    for (size_t i = 0; i < this->get_num_inputs(); i++)
    {
        this->input_msgs[i].clear();
        this->input_tags[i].clear();
    }

    //tell the upstream and downstram to re-check their tokens
    //this is how the other blocks know who is interested,
    //and can decide based on interest to set done or not
    for (size_t i = 0; i < this->get_num_inputs(); i++)
    {
        this->post_upstream(i, OutputCheckMessage());
    }
    for (size_t i = 0; i < this->get_num_outputs(); i++)
    {
        this->post_downstream(i, InputCheckMessage());
    }

    if (ARMAGEDDON) std::cerr
        << "==================================================\n"
        << "== The " << block_ptr->to_string() << " is done...\n"
        << "==================================================\n"
        << std::flush;
}
