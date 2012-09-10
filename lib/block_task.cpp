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
// along with io_sig program.  If not, see <http://www.gnu.org/licenses/>.

#include "element_impl.hpp"
#include <boost/foreach.hpp>
#include <algorithm>

#define REALLY_BIG size_t(1 << 30)

using namespace gnuradio;

void ElementImpl::mark_done(const tsbe::TaskInterface &task_iface)
{
    if (this->block_state == BLOCK_STATE_DONE) return; //can re-enter checking done first

    //flush partial output buffers to the downstream
    for (size_t i = 0; i < task_iface.get_num_outputs(); i++)
    {
        if (this->output_bytes_offset[i] == 0) continue;
        ASSERT(this->output_queues.ready(i));
        tsbe::Buffer &buff = this->output_queues.front(i);
        buff.get_length() = this->output_bytes_offset[i];
        task_iface.post_downstream(i, buff);
        this->output_queues.pop(i);
        this->output_bytes_offset[i] = 0;
    }

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
    for (size_t i = 0; i < task_iface.get_num_inputs(); i++)
    {
        task_iface.post_upstream(i, CheckTokensMessage());
    }
    for (size_t i = 0; i < task_iface.get_num_outputs(); i++)
    {
        task_iface.post_downstream(i, CheckTokensMessage());
    }

    if (ARMAGEDDON) std::cout
        << "==================================================\n"
        << "== The " << name << " is done...\n"
        << "==================================================\n"
        << std::flush;
}

void ElementImpl::handle_task(const tsbe::TaskInterface &task_iface)
{
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
    if (WORK) std::cout << "=== calling work on " << name << " ===" << std::endl;

    const size_t num_inputs = task_iface.get_num_inputs();
    const size_t num_outputs = task_iface.get_num_outputs();
    //const bool is_source = (num_inputs == 0);
    //const bool is_sink = (num_outputs == 0);
    this->work_io_ptr_mask = 0; //reset

    //------------------------------------------------------------------
    //-- sort the input tags before working
    //------------------------------------------------------------------
    for (size_t i = 0; i < num_inputs; i++)
    {
        if (not this->input_tags_changed[i]) continue;
        std::vector<Tag> &tags_i = this->input_tags[i];
        std::sort(tags_i.begin(), tags_i.end(), Tag::offset_compare);
        this->input_tags_changed[i] = false;
    }

    //------------------------------------------------------------------
    //-- initialize input buffers before work
    //------------------------------------------------------------------
    size_t num_input_items = REALLY_BIG; //so big that it must std::min
    size_t input_tokens_count = 0;
    for (size_t i = 0; i < num_inputs; i++)
    {
        input_tokens_count += this->input_tokens[i].use_count();

        ASSERT(this->input_queues.ready(i));
        const BuffInfo info = this->input_queues.front(i);
        const size_t items = info.len/this->input_items_sizes[i];

        this->work_io_ptr_mask |= ptrdiff_t(info.mem);
        this->input_items[i]._mem = info.mem;
        this->input_items[i]._len = items;
        this->work_input_items[i] = info.mem;
        this->work_ninput_items[i] = items;
        num_input_items = std::min(num_input_items, items);
    }

    //------------------------------------------------------------------
    //-- initialize output buffers before work
    //------------------------------------------------------------------
    size_t num_output_items = REALLY_BIG; //so big that it must std::min
    size_t output_tokens_count = 0;
    for (size_t i = 0; i < num_outputs; i++)
    {
        output_tokens_count += this->output_tokens[i].use_count();

        ASSERT(this->output_queues.ready(i));
        const tsbe::Buffer &buff = this->output_queues.front(i);
        char *mem = ((char *)buff.get_memory()) + this->output_bytes_offset[i];
        const size_t bytes = buff.get_length() - this->output_bytes_offset[i];
        const size_t items = bytes/this->output_items_sizes[i];

        this->work_io_ptr_mask |= ptrdiff_t(mem);
        this->output_items[i]._mem = mem;
        this->output_items[i]._len = items;
        this->work_output_items[i] = mem;
        num_output_items = std::min(num_output_items, items);
    }

    //if we have outputs and at least one port has no downstream subscibers, mark done
    if ((num_outputs != 0 and output_tokens_count == num_outputs))
    {
        this->mark_done(task_iface);
        return;
    }

    //------------------------------------------------------------------
    //-- forecast
    //------------------------------------------------------------------
    forecast_again_you_jerk:
    if (not this->enable_fixed_rate)
    {
        block_ptr->forecast(num_output_items, work_ninput_items);
        for (size_t i = 0; i < num_inputs; i++)
        {
            if (size_t(work_ninput_items[i]) > this->input_items[i].size())
            {
                for (size_t j = 0; j < num_inputs; j++)
                {
                    work_ninput_items[j] = this->input_items[j]._len;
                }
                num_output_items = num_output_items/2;
                if (num_output_items == 0)
                {
                    this->forecast_fail = true;
                    //TODO FIXME this logic is totally duplicated from the bottom!
                    //re-use some common code
                    {
                        if (num_inputs != 0 and input_tokens_count == num_inputs)
                        {
                            this->block.post_msg(CheckTokensMessage());
                            return;
                        }
                        if (this->input_queues.all_ready() and this->output_queues.all_ready())
                        {
                            this->block.post_msg(SelfKickMessage());
                            return;
                        }
                    }
                    return;
                }
                goto forecast_again_you_jerk;
            }
        }
    }
    this->forecast_fail = false;

    //------------------------------------------------------------------
    //-- the work
    //------------------------------------------------------------------
    work_noutput_items = num_output_items;
    if (this->enable_fixed_rate) work_noutput_items = std::min(
        work_noutput_items, myulround((num_input_items)*this->relative_rate));
    const int ret = block_ptr->Work(this->input_items, this->output_items);
    const size_t noutput_items = size_t(ret);

    if (ret == Block::WORK_DONE)
    {
        this->mark_done(task_iface);
        return;
    }

    //------------------------------------------------------------------
    //-- process input consumption
    //------------------------------------------------------------------
    bool input_allows_flush = true;
    for (size_t i = 0; i < num_inputs; i++)
    {
        ASSERT(enable_fixed_rate or ret != Block::WORK_CALLED_PRODUCE);
        const size_t items = (enable_fixed_rate)? (myulround((noutput_items/this->relative_rate))) : this->consume_items[i];
        this->consume_items[i] = 0;

        this->items_consumed[i] += items;
        const size_t bytes = items*this->input_items_sizes[i];
        input_allows_flush = input_allows_flush and this->input_queues.consume(i, bytes);
    }

    //------------------------------------------------------------------
    //-- process output production
    //------------------------------------------------------------------
    for (size_t i = 0; i < num_outputs; i++)
    {
        const size_t items = (ret == Block::WORK_CALLED_PRODUCE)? this->produce_items[i] : noutput_items;
        this->produce_items[i] = 0;
        if (items == 0) continue;

        this->items_produced[i] += items;
        const size_t bytes = items*this->output_items_sizes[i];
        this->output_bytes_offset[i] += bytes;

        //only pass output buffer downstream when the input is fully consumed...
        //Reasoning: For the sake of dealling with history, we can process the mini history input buffer,
        //and then call work again on the real input buffer, but still yield one output buffer per input buffer.
        if (input_allows_flush)
        {
            tsbe::Buffer &buff = this->output_queues.front(i);
            buff.get_length() = this->output_bytes_offset[i];
            task_iface.post_downstream(i, buff);
            this->output_queues.pop(i);
            this->output_bytes_offset[i] = 0;
        }
    }

    //------------------------------------------------------------------
    //-- trim the input tags that are past the consumption zone
    //-- and post trimmed tags to the downstream based on policy
    //------------------------------------------------------------------
    for (size_t i = 0; i < num_inputs; i++)
    {
        std::vector<Tag> &tags_i = this->input_tags[i];
        const size_t items_consumed_i = this->items_consumed[i];
        size_t last = 0;
        while (last < tags_i.size() and tags_i[last].offset < items_consumed_i)
        {
            last++;
        }

        //follow the tag propagation policy before erasure
        switch (this->tag_prop_policy)
        {
        case Block::TPP_DONT: break; //well that was ez
        case Block::TPP_ALL_TO_ALL:
            for (size_t out_i = 0; out_i < num_outputs; out_i++)
            {
                for (size_t tag_i = 0; tag_i < last; tag_i++)
                {
                    Tag t = tags_i[tag_i];
                    t.offset = myullround(t.offset * this->relative_rate);
                    task_iface.post_downstream(out_i, t);
                }
            }
            break;
        case Block::TPP_ONE_TO_ONE:
            if (i < num_outputs)
            {
                for (size_t tag_i = 0; tag_i < last; tag_i++)
                {
                    Tag t = tags_i[tag_i];
                    t.offset = myullround(t.offset * this->relative_rate);
                    task_iface.post_downstream(i, t);
                }
            }
            break;
        };

        //now its safe to perform the erasure
        if (last != 0) tags_i.erase(tags_i.begin(), tags_i.begin()+last);
    }

    //------------------------------------------------------------------
    //-- now commit all tags in the output queue to the downstream
    //------------------------------------------------------------------
    for (size_t i = 0; i < num_outputs; i++)
    {
        BOOST_FOREACH(const Tag &t, this->output_tags[i])
        {
            task_iface.post_downstream(i, t);
        }
        this->output_tags[i].clear();
    }

    //if there are inputs, and not all are provided for,
    //tell the block to check input queues and handle done
    if (num_inputs != 0 and input_tokens_count == num_inputs)
    {
        this->block.post_msg(CheckTokensMessage());
        return;
    }

    //still have IO ready? kick off another task
    if (this->input_queues.all_ready() and this->output_queues.all_ready())
    {
        this->block.post_msg(SelfKickMessage());
    }
}
