// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras_impl/block_actor.hpp>
#include "tag_handlers.hpp"

using namespace gras;

void BlockActor::task_main(void)
{
    TimerAccumulate ta_prep(this->stats.total_time_prep);

    //------------------------------------------------------------------
    //-- Decide if its possible to continue any processing:
    //-- Handle task may get called for incoming buffers,
    //-- however, not all ports may have available buffers.
    //------------------------------------------------------------------
    if GRAS_UNLIKELY(not this->is_work_allowed()) return;

    const size_t num_inputs = this->get_num_inputs();
    const size_t num_outputs = this->get_num_outputs();

    //------------------------------------------------------------------
    //-- initialize input buffers before work
    //------------------------------------------------------------------
    size_t output_inline_index = 0;
    this->input_items.min() = ~0;
    this->input_items.max() = 0;
    for (size_t i = 0; i < num_inputs; i++)
    {
        this->sort_tags(i);
        this->num_input_msgs_read[i] = 0;

        ASSERT(this->input_queues.ready(i));
        const SBuffer &buff = this->input_queues.front(i);
        const void *mem = buff.get();
        size_t items = buff.length/this->input_configs[i].item_size;

        this->input_items.vec()[i] = mem;
        this->input_items[i].get() = mem;
        this->input_items[i].size() = items;
        this->input_items.min() = std::min(this->input_items.min(), items);
        this->input_items.max() = std::max(this->input_items.max(), items);

        //inline dealings, how and when input buffers can be inlined into output buffers
        //*
        if GRAS_UNLIKELY(
            buff.unique() and
            input_configs[i].inline_buffer and
            output_inline_index < num_outputs and
            buff.get_affinity() == this->buffer_affinity
        ){
            this->output_queues.set_inline(output_inline_index++, buff);
        }
        //*/
    }

    //------------------------------------------------------------------
    //-- initialize output buffers before work
    //------------------------------------------------------------------
    this->output_items.min() = ~0;
    this->output_items.max() = 0;
    for (size_t i = 0; i < num_outputs; i++)
    {
        ASSERT(this->output_queues.ready(i));
        SBuffer &buff = this->output_queues.front(i);
        ASSERT(buff.length == 0); //assumes it was flushed last call
        void *mem = buff.get();
        const size_t bytes = buff.get_actual_length() - buff.offset;
        size_t items = bytes/this->output_configs[i].item_size;

        this->output_items.vec()[i] = mem;
        this->output_items[i].get() = mem;
        this->output_items[i].size() = items;
        this->output_items.min() = std::min(this->output_items.min(), items);
        this->output_items.max() = std::max(this->output_items.max(), items);
    }

    //------------------------------------------------------------------
    //-- the work
    //------------------------------------------------------------------
    ta_prep.done();
    this->stats.work_count++;
    if GRAS_UNLIKELY(this->interruptible_thread)
    {
        TimerAccumulate ta_work(this->stats.total_time_work);
        this->interruptible_thread->call();
    }
    else
    {
        TimerAccumulate ta_work(this->stats.total_time_work);
        this->task_work();
    }
    this->stats.time_last_work = time_now();

    //------------------------------------------------------------------
    //-- Post-work output tasks
    //------------------------------------------------------------------
    for (size_t i = 0; i < num_outputs; i++)
    {
        //buffer may be popped by one of the special buffer api hooks
        if GRAS_UNLIKELY(this->output_queues.empty(i)) continue;

        //grab a copy of the front buffer then consume from the queue
        InputBufferMessage buff_msg;
        buff_msg.buffer = this->output_queues.front(i);
        this->output_queues.consume(i);

        //Post a buffer message downstream only if the produce flag was marked.
        //So this explicitly after consuming the output queues so pop is called.
        //This is because pop may have special hooks in it to prepare the buffer.
        
    TimerAccumulate ta_post(this->stats.total_time_post);
        if GRAS_LIKELY(this->produce_outputs[i]) this->post_downstream(i, buff_msg);
        ta_post.done();
        this->produce_outputs[i] = false;
    }

    //------------------------------------------------------------------
    //-- Post-work input tasks
    //------------------------------------------------------------------
    for (size_t i = 0; i < num_inputs; i++)
    {
        this->trim_msgs(i);

    TimerAccumulate ta_post(this->stats.total_time_post);
        this->input_queues.pop(i);
        ta_post.done();

        //update the inputs available bit field
        this->update_input_avail(i);

        //missing at least one upstream provider?
        //since nothing else is coming in, its safe to mark done
        if GRAS_UNLIKELY(this->is_input_done(i)) this->mark_done();
    }

    //still have IO ready? kick off another task
    this->task_kicker();
}
