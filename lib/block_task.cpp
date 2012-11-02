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

using namespace gras;

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

GRAS_FORCE_INLINE void BlockActor::input_fail(const size_t i)
{
    //input failed, accumulate and try again
    if (not this->input_queues.is_accumulated(i))
    {
        this->input_queues.accumulate(i, this->input_items_sizes[i]);
        this->Push(SelfKickMessage(), Theron::Address());
        return;
    }
    //otherwise check for done, else wait for more
    if (this->inputs_done[i]) this->mark_done();
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
    size_t output_inline_index = 0;
    for (size_t i = 0; i < num_inputs; i++)
    {
        this->sort_tags(i);

        ASSERT(this->input_queues.ready(i));
        //this->input_queues.accumulate(i, this->input_items_sizes[i]);
        const SBuffer &buff = this->input_queues.front(i);
        void *mem = buff.get();
        size_t items = buff.length/this->input_items_sizes[i];

        this->input_items[i].get() = mem;
        this->input_items[i].size() = items;

        this->consume_items[i] = 0;

        //inline dealings, how and when input buffers can be inlined into output buffers
        //continue;
        if (
            buff.unique() and
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
    for (size_t i = 0; i < num_outputs; i++)
    {
        ASSERT(this->output_queues.ready(i));
        const SBuffer &buff = this->output_queues.front(i);
        void *mem = buff.get(buff.length);
        const size_t bytes = buff.get_actual_length() - buff.length - buff.offset;
        size_t items = bytes/this->output_items_sizes[i];

        this->output_items[i].get() = mem;
        this->output_items[i].size() = items;

        this->produce_items[i] = 0;
    }

    //------------------------------------------------------------------
    //-- the work
    //------------------------------------------------------------------
    this->work_ret = -1;
    if (this->interruptible_thread)
    {
        this->interruptible_thread->call();
    }
    else
    {
        this->task_work();
    }

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
        const size_t items = this->consume_items[i];

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
        const size_t items = this->produce_items[i];
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
