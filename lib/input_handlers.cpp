// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras_impl/block_actor.hpp>
#include <boost/foreach.hpp>

using namespace gras;

void BlockActor::handle_input_tag(const InputTagMessage &message, const Theron::Address)
{
    TimerAccumulate ta(this->stats.total_time_input);
    MESSAGE_TRACER();
    const size_t index = message.index;

    //handle incoming stream tag, push into the tag storage
    if (this->block_state == BLOCK_STATE_DONE) return;
    this->input_tags[index].push_back(message.tag);
    this->input_tags_changed[index] = true;
}

void BlockActor::handle_input_msg(const InputMsgMessage &message, const Theron::Address)
{
    TimerAccumulate ta(this->stats.total_time_input);
    MESSAGE_TRACER();
    const size_t index = message.index;

    //handle incoming async message, push into the msg storage
    if (this->block_state == BLOCK_STATE_DONE) return;
    this->input_msgs[index].push_back(message.msg);
    this->update_input_avail(index);

    ta.done();
    this->task_main();
}

void BlockActor::handle_input_buffer(const InputBufferMessage &message, const Theron::Address)
{
    TimerAccumulate ta(this->stats.total_time_input);
    MESSAGE_TRACER();
    const size_t index = message.index;

    //handle incoming stream buffer, push into the queue
    if (this->block_state == BLOCK_STATE_DONE) return;
    this->input_queues.push(index, message.buffer);
    this->update_input_avail(index);

    ta.done();
    this->task_main();
}

void BlockActor::handle_input_token(const InputTokenMessage &message, const Theron::Address)
{
    TimerAccumulate ta(this->stats.total_time_input);
    MESSAGE_TRACER();
    ASSERT(message.index < this->get_num_inputs());

    //store the token of the upstream producer
    this->token_pool.insert(message.token);
}

void BlockActor::handle_input_check(const InputCheckMessage &message, const Theron::Address)
{
    TimerAccumulate ta(this->stats.total_time_input);
    MESSAGE_TRACER();
    const size_t index = message.index;

    //an upstream block declared itself done, recheck the token
    this->inputs_done.set(index, this->input_tokens[index].unique());

    //upstream done, give it one more attempt at task handling
    ta.done();
    this->task_main();

    //now recheck the status, mark block done if the input is done
    if (this->is_input_done(index)) this->mark_done();
}

void BlockActor::handle_input_alloc(const InputAllocMessage &message, const Theron::Address)
{
    TimerAccumulate ta(this->stats.total_time_input);
    MESSAGE_TRACER();
    const size_t index = message.index;

    //handle the upstream block allocation request
    OutputAllocMessage new_msg;
    new_msg.queue = block_ptr->input_buffer_allocator(index, message.config);
    if (new_msg.queue) this->post_upstream(index, new_msg);
}

void BlockActor::handle_input_update(const InputUpdateMessage &message, const Theron::Address)
{
    TimerAccumulate ta(this->stats.total_time_input);
    MESSAGE_TRACER();
    const size_t i = message.index;

    //update buffer queue configuration
    if (i >= this->input_queues.size()) return;
    const size_t preload_bytes = this->input_configs[i].item_size*this->input_configs[i].preload_items;
    const size_t reserve_bytes = this->input_configs[i].item_size*this->input_configs[i].reserve_items;
    const size_t maximum_bytes = this->input_configs[i].item_size*this->input_configs[i].maximum_items;
    this->input_queues.update_config(i, this->input_configs[i].item_size, preload_bytes, reserve_bytes, maximum_bytes);
}
