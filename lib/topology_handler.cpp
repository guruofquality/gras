// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras_impl/block_actor.hpp>

using namespace gras;

template <typename V, typename T>
void resize_fill_grow(V &v, const size_t new_len, const T &fill)
{
    if (v.size() >= new_len) return; //dont ever shrink it
    v.resize(new_len, fill);
}

template <typename V>
void resize_fill_back(V &v, const size_t new_len)
{
    if (v.empty()) v.resize(1);
    resize_fill_grow(v, new_len, v.back());
}

void BlockActor::handle_topology(
    const Apology::WorkerTopologyMessage &,
    const Theron::Address from
){
    MESSAGE_TRACER();

    const size_t num_inputs = worker->get_num_inputs();
    const size_t num_outputs = worker->get_num_outputs();

    //call notify_topology on block before committing settings
    data->block->notify_topology(num_inputs, num_outputs);

    //resize and fill port properties
    resize_fill_back(data->input_configs, num_inputs);
    resize_fill_back(data->output_configs, num_outputs);

    //resize the bytes consumed/produced
    resize_fill_grow(data->stats.items_consumed, num_inputs, 0);
    resize_fill_grow(data->stats.tags_consumed, num_inputs, 0);
    resize_fill_grow(data->stats.msgs_consumed, num_inputs, 0);
    resize_fill_grow(data->stats.items_produced, num_outputs, 0);
    resize_fill_grow(data->stats.tags_produced, num_outputs, 0);
    resize_fill_grow(data->stats.msgs_produced, num_outputs, 0);

    //resize all work buffers to match current connections
    data->input_items.resize(num_inputs);
    data->output_items.resize(num_outputs);
    data->input_queues.resize(num_inputs);
    data->output_queues.resize(num_outputs);
    data->inputs_available.resize(num_inputs);
    data->total_items_consumed.resize(num_inputs);
    data->total_items_produced.resize(num_outputs);
    if (num_inputs == 0) data->inputs_available.resize(1, true); //so its always "available"

    //copy the name into the queues for debug purposes
    data->input_queues.name = this->name;
    data->output_queues.name = this->name;

    //resize the token trackers
    data->input_tokens.resize(num_inputs);
    data->output_tokens.resize(num_outputs);
    data->inputs_done.resize(num_inputs);
    data->outputs_done.resize(num_outputs);
    data->output_allocation_hints.resize(num_outputs);

    //resize tags vector to match sizes
    data->input_tags_changed.resize(num_inputs);
    data->input_tags.resize(num_inputs);
    data->num_input_msgs_read.resize(num_inputs);
    data->num_input_items_read.resize(num_inputs);
    data->num_output_items_read.resize(num_outputs);
    data->input_msgs.resize(num_inputs);

    //a block looses all connections, allow it to free
    if (num_inputs == 0 and num_outputs == 0)
    {
        this->mark_done();
    }

    for (size_t i = 0; i < num_inputs; i++)
    {
        InputUpdateMessage message;
        message.index = i;
        this->handle_input_update(message, Theron::Address());
    }
    for (size_t i = 0; i < num_outputs; i++)
    {
        OutputUpdateMessage message;
        message.index = i;
        this->handle_output_update(message, Theron::Address());
    }

    this->Send(0, from); //ACK
}
