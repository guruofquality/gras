// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras_impl/block_actor.hpp>
#include "tag_handlers.hpp"

using namespace gras;

void BlockActor::task_main(void)
{
    TimerAccumulate ta_prep(data->stats.total_time_prep);

    //------------------------------------------------------------------
    //-- Decide if its possible to continue any processing:
    //-- Handle task may get called for incoming buffers,
    //-- however, not all ports may have available buffers.
    //------------------------------------------------------------------
    if GRAS_UNLIKELY(not this->is_work_allowed()) return;

    const size_t num_inputs = worker->get_num_inputs();
    const size_t num_outputs = worker->get_num_outputs();

    //------------------------------------------------------------------
    //-- initialize input buffers before work
    //------------------------------------------------------------------
    size_t output_inline_index = 0;
    data->input_items.min() = ~0;
    data->input_items.max() = 0;
    for (size_t i = 0; i < num_inputs; i++)
    {
        this->sort_tags(i);
        data->num_input_msgs_read[i] = 0;

        ASSERT(data->input_queues.ready(i));
        const SBuffer &buff = data->input_queues.front(i);
        const void *mem = buff.get();
        size_t items = buff.length/data->input_configs[i].item_size;

        data->input_items.vec()[i] = mem;
        data->input_items[i].get() = mem;
        data->input_items[i].size() = items;
        data->input_items.min() = std::min(data->input_items.min(), items);
        data->input_items.max() = std::max(data->input_items.max(), items);

        //inline dealings, how and when input buffers can be inlined into output buffers
        //*
        if GRAS_UNLIKELY(
            buff.unique() and
            data->input_configs[i].inline_buffer and
            output_inline_index < num_outputs and
            buff.get_affinity() == data->buffer_affinity
        ){
            data->output_queues.set_inline(output_inline_index++, buff);
        }
        //*/
    }

    //------------------------------------------------------------------
    //-- initialize output buffers before work
    //------------------------------------------------------------------
    data->output_items.min() = ~0;
    data->output_items.max() = 0;
    for (size_t i = 0; i < num_outputs; i++)
    {
        ASSERT(data->output_queues.ready(i));
        SBuffer &buff = data->output_queues.front(i);
        ASSERT(buff.length == 0); //assumes it was flushed last call
        void *mem = buff.get();
        const size_t bytes = buff.get_actual_length() - buff.offset;
        size_t items = bytes/data->output_configs[i].item_size;

        data->output_items.vec()[i] = mem;
        data->output_items[i].get() = mem;
        data->output_items[i].size() = items;
        data->output_items.min() = std::min(data->output_items.min(), items);
        data->output_items.max() = std::max(data->output_items.max(), items);
    }

    //------------------------------------------------------------------
    //-- the work
    //------------------------------------------------------------------
    ta_prep.done();
    data->stats.work_count++;
    if GRAS_UNLIKELY(data->interruptible_thread)
    {
        TimerAccumulate ta_work(data->stats.total_time_work);
        data->interruptible_thread->call();
    }
    else
    {
        TimerAccumulate ta_work(data->stats.total_time_work);
        this->task_work();
    }
    data->stats.time_last_work = time_now();
    TimerAccumulate ta_post(data->stats.total_time_post);

    //------------------------------------------------------------------
    //-- Post-work output tasks
    //------------------------------------------------------------------
    for (size_t i = 0; i < num_outputs; i++)
    {
        //buffer may be popped by one of the special buffer api hooks
        if GRAS_UNLIKELY(data->output_queues.empty(i)) continue;

        //grab a copy of the front buffer then consume from the queue
        InputBufferMessage buff_msg;
        buff_msg.buffer = data->output_queues.front(i);
        data->output_queues.consume(i);

        //Post a buffer message downstream only if the produce flag was marked.
        //So this explicitly after consuming the output queues so pop is called.
        //This is because pop may have special hooks in it to prepare the buffer.
        if GRAS_LIKELY(data->produce_outputs[i]) worker->post_downstream(i, buff_msg);
        data->produce_outputs[i] = false;
    }

    //------------------------------------------------------------------
    //-- Post-work input tasks
    //------------------------------------------------------------------
    for (size_t i = 0; i < num_inputs; i++)
    {
        this->trim_msgs(i);

        //update the inputs available bit field
        this->update_input_avail(i);

        //missing at least one upstream provider?
        //since nothing else is coming in, its safe to mark done
        if GRAS_UNLIKELY(this->is_input_done(i)) this->mark_done();
    }

    //still have IO ready? kick off another task
    this->task_kicker();
}
