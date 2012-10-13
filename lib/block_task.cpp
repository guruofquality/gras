//
// Copyright 2012 Josh Blum
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <gras_impl/block_actor.hpp>
#include "tag_handlers.hpp"

#define REALLY_BIG size_t(1 << 30)

using namespace gnuradio;

void BlockActor::mark_done(void)
{
    if (this->block_state == BLOCK_STATE_DONE) return; //can re-enter checking done first

    this->block_ptr->stop();

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

    //release allocator tokens, buffers can now call deleters
    this->output_buffer_tokens.clear();

    //release all buffers in queues
    this->input_queues.flush_all();
    this->output_queues.flush_all();

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

void BlockActor::handle_task(void)
{
    #ifdef WORK_DEBUG
    WorkDebugPrinter WDP(block_ptr->to_string());
    #endif

    //------------------------------------------------------------------
    //-- Decide if its possible to continue any processing:
    //-- Handle task may get called for incoming buffers,
    //-- however, not all ports may have available buffers.
    //------------------------------------------------------------------
    if (not(
        this->block_state == BLOCK_STATE_LIVE and
        this->input_queues.all_ready() and
        this->output_queues.all_ready()
    )) return;

    const size_t num_inputs = this->get_num_inputs();
    const size_t num_outputs = this->get_num_outputs();
    this->work_io_ptr_mask = 0; //reset

    //------------------------------------------------------------------
    //-- initialize input buffers before work
    //------------------------------------------------------------------
    size_t num_input_items = REALLY_BIG; //so big that it must std::min
    size_t output_inline_index = 0;
    for (size_t i = 0; i < num_inputs; i++)
    {
        this->sort_tags(i);

        ASSERT(this->input_queues.ready(i));
        bool potential_inline;
        const SBuffer buff = this->input_queues.front(i, this->enable_fixed_rate and num_outputs, potential_inline);
        void *mem = buff.get();
        size_t items = buff.length/this->input_items_sizes[i];

        this->work_io_ptr_mask |= ptrdiff_t(mem);
        this->input_items[i].get() = mem;
        this->input_items[i].size() = items;
        this->work_input_items[i] = mem;
        this->work_ninput_items[i] = items;

        if (this->enable_fixed_rate)
        {
            if (items <= this->input_configs[i].lookahead_items)
            {
                if (this->inputs_done[i]) this->mark_done();
                return;
            }
            items -= this->input_configs[i].lookahead_items;
        }

        num_input_items = std::min(num_input_items, items);
        this->consume_items[i] = 0;
        this->consume_called[i] = false;

        //inline dealings, how and when input buffers can be inlined into output buffers
        //continue;
        if (
            potential_inline and
            input_configs[i].inline_buffer and
            output_inline_index < num_outputs and
            buff.get_affinity() == this->buffer_affinity
        ){
            //copy buffer reference but push with zero length, same offset
            SBuffer new_obuff = buff;
            new_obuff.length = 0;
            this->output_queues.push_front(i, new_obuff); //you got inlined!
            output_inline_index++; //done do this output port again
        }
    }

    //------------------------------------------------------------------
    //-- initialize output buffers before work
    //------------------------------------------------------------------
    size_t num_output_items = REALLY_BIG; //so big that it must std::min
    for (size_t i = 0; i < num_outputs; i++)
    {
        ASSERT(this->output_queues.ready(i));
        const SBuffer &buff = this->output_queues.front(i);
        void *mem = buff.get(buff.length);
        const size_t bytes = buff.get_actual_length() - buff.length - buff.offset;
        size_t items = bytes/this->output_items_sizes[i];

        this->work_io_ptr_mask |= ptrdiff_t(mem);
        this->output_items[i].get() = mem;
        this->output_items[i].size() = items;
        this->work_output_items[i] = mem;

        items /= this->output_multiple_items;
        items *= this->output_multiple_items;
        num_output_items = std::min(num_output_items, items);
        this->produce_items[i] = 0;
    }

    //------------------------------------------------------------------
    //-- calculate the work_noutput_items given:
    //-- min of num_input_items
    //-- min of num_output_items
    //-- relative rate and output multiple items
    //------------------------------------------------------------------
    work_noutput_items = num_output_items;
    if (num_inputs and (this->enable_fixed_rate or not num_outputs))
    {
        size_t calc_output_items = size_t(num_input_items*this->relative_rate);
        calc_output_items += this->output_multiple_items-1;
        calc_output_items /= this->output_multiple_items;
        calc_output_items *= this->output_multiple_items;
        if (calc_output_items and calc_output_items < work_noutput_items)
            work_noutput_items = calc_output_items;
    }

    //------------------------------------------------------------------
    //-- forecast
    //------------------------------------------------------------------
    VAR(work_noutput_items);
    if (this->forecast_enable)
    {
        forecast_again_you_jerk:
        fcast_ninput_items = work_ninput_items; //init for NOP case
        block_ptr->forecast(work_noutput_items, fcast_ninput_items);
        for (size_t i = 0; i < num_inputs; i++)
        {
            if (fcast_ninput_items[i] <= work_ninput_items[i]) continue;

            const size_t work_noutput_items_last = work_noutput_items;
            work_noutput_items = work_noutput_items/2; //backoff regime
            work_noutput_items += this->output_multiple_items-1;
            work_noutput_items /= this->output_multiple_items;
            work_noutput_items *= this->output_multiple_items;
            if (work_noutput_items and work_noutput_items_last != work_noutput_items) goto forecast_again_you_jerk;

            //handle the case of forecast failing
            //TODO accumulate input here, only done if inputs done and already accumulated
            if (this->inputs_done[i]) this->mark_done();
            return;
        }
    }

    //------------------------------------------------------------------
    //-- the work
    //------------------------------------------------------------------
    VAR(work_noutput_items);
    if (num_inputs) VAR(work_ninput_items[0]);
    this->work_ret = -1;
    if (this->interruptible_thread)
    {
        this->interruptible_thread->call();
    }
    else
    {
        this->task_work();
    }
    const size_t noutput_items = size_t(work_ret);
    VAR(work_ret);

    if (work_ret == Block::WORK_DONE)
    {
        this->mark_done();
        return;
    }

    //------------------------------------------------------------------
    //-- process input consumption
    //------------------------------------------------------------------
    for (size_t i = 0; i < num_inputs; i++)
    {
        ASSERT(enable_fixed_rate or work_ret != Block::WORK_CALLED_PRODUCE);
        const bool use_consume = (not this->enable_fixed_rate) or (this->consume_called[i]);
        const size_t items = (use_consume)? this->consume_items[i] : (myulround((noutput_items/this->relative_rate)));

        this->items_consumed[i] += items;
        const size_t bytes = items*this->input_items_sizes[i];
        this->input_queues.consume(i, bytes);

        this->trim_tags(i);
    }

    //------------------------------------------------------------------
    //-- process output production
    //------------------------------------------------------------------
    for (size_t i = 0; i < num_outputs; i++)
    {
        const size_t items = (work_ret == Block::WORK_CALLED_PRODUCE)? this->produce_items[i] : noutput_items;
        if (items == 0) continue;

        SBuffer &buff = this->output_queues.front(i);
        this->items_produced[i] += items;
        const size_t bytes = items*this->output_items_sizes[i];
        buff.length += bytes;

        //dont always pass output buffers downstream for the sake of efficiency
        if (not this->input_queues.all_ready() or buff.length*2 > buff.get_actual_length())
        {
            InputBufferMessage buff_msg;
            buff_msg.buffer = buff;
            this->post_downstream(i, buff_msg);
            this->output_queues.pop(i);
        }
    }

    //------------------------------------------------------------------
    //-- Message self based on post-work conditions
    //------------------------------------------------------------------
    //missing at least one upstream provider?
    //since nothing else is coming in, its safe to mark done
    if (this->any_inputs_done()) this->mark_done();

    //still have IO ready? kick off another task
    if (this->input_queues.all_ready() and this->output_queues.all_ready())
    {
        this->Push(SelfKickMessage(), Theron::Address());
    }
}
