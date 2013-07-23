// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras_impl/block_actor.hpp>
#include <boost/foreach.hpp>

using namespace gras;

void BlockActor::handle_input_tag(const InputTagMessage &message, const Theron::Address)
{
    TimerAccumulate ta(data->stats.total_time_input);
    MESSAGE_TRACER();
    const size_t index = message.index;

    //handle incoming stream tag, push into the tag storage
    if GRAS_UNLIKELY(data->block_state == BLOCK_STATE_DONE) return;
    data->input_tags[index].push_back(message.tag);
    data->input_tags_changed[index] = true;
}

void BlockActor::handle_input_msg(const InputMsgMessage &message, const Theron::Address)
{
    TimerAccumulate ta(data->stats.total_time_input);
    MESSAGE_TRACER();
    const size_t index = message.index;

    //handle incoming async message, push into the msg storage
    if GRAS_UNLIKELY(data->block_state == BLOCK_STATE_DONE) return;
    data->input_msgs[index].push_back(message.msg);
    this->update_input_avail(index);

    ta.done();
    this->task_main();
}

void BlockActor::handle_input_buffer(const InputBufferMessage &message, const Theron::Address)
{
    TimerAccumulate ta(data->stats.total_time_input);
    MESSAGE_TRACER();
    const size_t index = message.index;

    //handle incoming stream buffer, push into the queue
    if GRAS_UNLIKELY(data->block_state == BLOCK_STATE_DONE) return;
    data->input_queues.push(index, message.buffer);
    this->update_input_avail(index);

    ta.done();
    this->task_main();
}

void BlockActor::handle_input_token(const InputTokenMessage &message, const Theron::Address)
{
    TimerAccumulate ta(data->stats.total_time_input);
    MESSAGE_TRACER();
    ASSERT(message.index < worker->get_num_inputs());

    //store the token of the upstream producer
    data->token_pool.insert(message.token);
}

void BlockActor::handle_input_check(const InputCheckMessage &message, const Theron::Address)
{
    TimerAccumulate ta(data->stats.total_time_input);
    MESSAGE_TRACER();
    const size_t index = message.index;

    //record time of the first input declared done
    if (not data->inputs_done.any())
    {
        data->first_input_done_time = boost::get_system_time();
    }

    //an upstream block declared itself done, recheck the token
    data->inputs_done.set(index, data->input_tokens[index].unique());

    //upstream done, give it one more attempt at task handling
    ta.done();
    this->task_main();

    //Now check the status, mark block done if the input is done:
    //When reserve_items is non zero, this ports is a sync input;
    //mark the block done if the sync input will never be ready again.
    //Otherwise, only mark done if all inputs are done with no data.
    const size_t reserve_items = data->input_configs[index].reserve_items;
    if (
        (reserve_items != 0 and data->inputs_done[index] and not data->inputs_available[index])
        or (reserve_items == 0 and data->inputs_done.all() and data->inputs_available.none())
    ) this->mark_done();
}

void BlockActor::handle_input_alloc(const InputAllocMessage &message, const Theron::Address)
{
    TimerAccumulate ta(data->stats.total_time_input);
    MESSAGE_TRACER();
    const size_t index = message.index;

    //new token for this downstream allocator
    //so if the downstream overrides, it has a unique token
    SBufferDeleter deleter = *(message.token);
    SBufferToken token = SBufferToken(new SBufferDeleter(deleter));
    SBufferConfig config = message.config;
    config.token = token;

    //handle the upstream block allocation request
    OutputAllocMessage new_msg;
    new_msg.queue = data->block->input_buffer_allocator(index, config);
    if (new_msg.queue) worker->post_upstream(index, new_msg);
}

void BlockActor::handle_input_update(const InputUpdateMessage &message, const Theron::Address)
{
    TimerAccumulate ta(data->stats.total_time_input);
    MESSAGE_TRACER();
    const size_t i = message.index;

    //update buffer queue configuration
    if (i >= data->input_queues.size()) return;
    const size_t preload_bytes = data->input_configs[i].item_size*data->input_configs[i].preload_items;
    const size_t reserve_bytes = data->input_configs[i].item_size*data->input_configs[i].reserve_items;
    const size_t maximum_bytes = data->input_configs[i].item_size*data->input_configs[i].maximum_items;
    data->input_queues.update_config(i, data->input_configs[i].item_size, preload_bytes, reserve_bytes, maximum_bytes);
    this->update_input_avail(i);
}
