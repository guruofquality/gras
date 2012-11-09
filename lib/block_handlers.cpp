// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras_impl/block_actor.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>

using namespace gras;


void BlockActor::handle_top_active(
    const TopActiveMessage &,
    const Theron::Address from
){
    MESSAGE_TRACER();

    if (this->block_state != BLOCK_STATE_LIVE)
    {
        this->block_ptr->start();
    }
    this->block_state = BLOCK_STATE_LIVE;

    this->Push(SelfKickMessage(), Theron::Address());

    this->Send(0, from); //ACK
}

void BlockActor::handle_top_inert(
    const TopInertMessage &,
    const Theron::Address from
){
    MESSAGE_TRACER();

    this->mark_done();

    this->Send(0, from); //ACK
}

void BlockActor::handle_top_token(
    const TopTokenMessage &message,
    const Theron::Address from
){
    MESSAGE_TRACER();

    //create input tokens and send allocation hints
    for (size_t i = 0; i < this->get_num_inputs(); i++)
    {
        this->input_tokens[i] = Token::make();
        this->inputs_done.reset(i);
        OutputTokenMessage token_msg;
        token_msg.token = this->input_tokens[i];
        this->post_upstream(i, token_msg);

        //TODO, schedule this message as a pre-allocation message
        //tell the upstream about the input requirements
        OutputHintMessage output_hints;
        output_hints.history_bytes = this->input_configs[i].lookahead_items*this->input_items_sizes[i];
        output_hints.reserve_bytes = 1; //TODO what do we want here, and so we like these hints at all?
        output_hints.token = this->input_tokens[i];
        this->post_upstream(i, output_hints);

    }

    //create output token
    for (size_t i = 0; i < this->get_num_outputs(); i++)
    {
        this->output_tokens[i] = Token::make();
        this->outputs_done.reset(i);
        InputTokenMessage token_msg;
        token_msg.token = this->output_tokens[i];
        this->post_downstream(i, token_msg);
    }

    //store a token to the top level topology
    this->token_pool.insert(message.token);

    this->Send(0, from); //ACK
}

void BlockActor::handle_top_config(
    const GlobalBlockConfig &message,
    const Theron::Address from
){
    MESSAGE_TRACER();

    //overwrite with global config only if maxium_items is not set (zero)
    for (size_t i = 0; i < this->output_configs.size(); i++)
    {
        if (this->output_configs[i].maximum_items == 0)
        {
            this->output_configs[i].maximum_items = message.maximum_output_items;
        }
    }

    //overwrite with global node affinity setting for buffers if not set
    if (this->buffer_affinity == -1)
    {
        this->buffer_affinity = message.buffer_affinity;
    }

    this->Send(0, from); //ACK
}

void BlockActor::handle_top_thread_group(
    const SharedThreadGroup &message,
    const Theron::Address from
){
    MESSAGE_TRACER();

    //store the topology's thread group
    //erase any potentially old lingering threads
    //spawn a new thread if this block is a source
    this->thread_group = message;
    this->interruptible_thread.reset(); //erase old one
    if (this->interruptible_work)
    {
        this->interruptible_thread = boost::make_shared<InterruptibleThread>(
            this->thread_group, boost::bind(&BlockActor::task_work, this)
        );
    }

    this->Send(0, from); //ACK
}

void BlockActor::handle_self_kick(
    const SelfKickMessage &,
    const Theron::Address
){
    MESSAGE_TRACER();
    this->handle_task();
}
