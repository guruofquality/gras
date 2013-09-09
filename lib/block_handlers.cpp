// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras_impl/block_actor.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>

using namespace gras;


void BlockActor::handle_top_active(
    const TopActiveMessage &message,
    const Theron::Address from
){
    MESSAGE_TRACER();

    if (data->block_state != BLOCK_STATE_LIVE)
    {
        data->block->notify_active();
        data->stats.start_time = time_now();
    }
    data->block_state = BLOCK_STATE_LIVE;

    this->Send(0, from); //ACK

    //work could have been skipped by a high prio msg
    //forcefully kick the task to recheck in a new call
    this->Send(SelfKickMessage(), this->GetAddress());
}

void BlockActor::handle_top_inert(
    const TopInertMessage &,
    const Theron::Address from
){
    MESSAGE_TRACER();

    this->mark_done();

    if (from != Theron::Address::Null()) this->Send(0, from); //ACK
}

void BlockActor::handle_top_token(
    const TopTokenMessage &message,
    const Theron::Address from
){
    MESSAGE_TRACER();

    //create input tokens and send allocation hints
    for (size_t i = 0; i < worker->get_num_inputs(); i++)
    {
        data->input_tokens[i] = Token::make();
        data->inputs_done.reset(i);
        OutputTokenMessage token_msg;
        token_msg.token = data->input_tokens[i];
        worker->post_upstream(i, token_msg);

        //TODO, schedule this message as a pre-allocation message
        //tell the upstream about the input requirements
        OutputHintMessage output_hints;
        output_hints.reserve_bytes = data->input_configs[i].reserve_items*data->input_configs[i].item_size;
        output_hints.token = data->input_tokens[i];
        worker->post_upstream(i, output_hints);

    }

    //create output token
    for (size_t i = 0; i < worker->get_num_outputs(); i++)
    {
        data->output_tokens[i] = Token::make();
        data->outputs_done.reset(i);
        InputTokenMessage token_msg;
        token_msg.token = data->output_tokens[i];
        worker->post_downstream(i, token_msg);
    }

    //store a token to the top level topology
    data->token_pool.insert(message.token);

    this->Send(0, from); //ACK
}

void BlockActor::handle_top_config(
    const TopConfigMessage &message,
    const Theron::Address from
){
    MESSAGE_TRACER();

    //merge in the non-defaults
    data->block->global_config().merge(message.config);

    //overwrite with global config only if maxium_items is not set (zero)
    for (size_t i = 0; i < data->output_configs.size(); i++)
    {
        if (data->output_configs[i].maximum_items == 0)
        {
            data->output_configs[i].maximum_items = data->block->global_config().maximum_output_items;
        }
    }

    this->Send(0, from); //ACK
}

void BlockActor::handle_top_thread_group(
    const TopThreadMessage &message,
    const Theron::Address from
){
    MESSAGE_TRACER();

    //store the topology's thread group
    //erase any potentially old lingering threads
    //spawn a new thread if this block is a source
    data->thread_group = message.thread_group;
    data->interruptible_thread.reset(); //erase old one
    if (data->block->global_config().interruptible_work)
    {
        data->interruptible_thread = boost::make_shared<InterruptibleThread>(
            data->thread_group, boost::bind(&BlockActor::task_work, this)
        );
    }

    this->Send(0, from); //ACK
}

void BlockActor::handle_self_kick(
    const SelfKickMessage &,
    const Theron::Address
){
    MESSAGE_TRACER();
    this->task_main();
}

void BlockActor::handle_get_stats(
    const GetStatsMessage &,
    const Theron::Address from
){
    MESSAGE_TRACER();

    //instantaneous states we update here,
    //and not interleaved with the rest of the code
    const size_t num_inputs = worker->get_num_inputs();
    data->stats.items_enqueued.resize(num_inputs);
    data->stats.tags_enqueued.resize(num_inputs);
    data->stats.msgs_enqueued.resize(num_inputs);
    for (size_t i = 0; i < num_inputs; i++)
    {
        data->stats.items_enqueued[i] = data->input_queues.get_items_enqueued(i);
        data->stats.tags_enqueued[i] = data->input_tags[i].size();
        data->stats.msgs_enqueued[i] = data->input_msgs[i].size();
    }
    data->stats.actor_queue_depth = this->GetNumQueuedMessages();
    data->stats.bytes_copied = data->input_queues.bytes_copied;
    data->stats.inputs_idle = data->input_queues.total_idle_times;
    data->stats.outputs_idle = data->output_queues.total_idle_times;

    //create the message reply object
    GetStatsMessage message;
    message.block_id = data->block->get_uid();
    message.stats = data->stats;
    message.stats_time = time_now();

    this->Send(message, from); //ACK

    //work could have been skipped by a high prio msg
    //forcefully kick the task to recheck in a new call
    this->Send(SelfKickMessage(), this->GetAddress());
}
