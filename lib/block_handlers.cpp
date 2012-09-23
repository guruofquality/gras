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
#include <gras_impl/vector_utils.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>

using namespace gnuradio;

void ElementImpl::handle_block_msg(
    const tsbe::TaskInterface &task_iface,
    const tsbe::Wax &msg
){
    if (MESSAGE) std::cerr << "handle_block_msg (" << msg.type().name() << ") " << name << std::endl;

    //a buffer has returned from the downstream
    //(all interested consumers have finished with it)
    if (msg.type() == typeid(BufferReturnMessage))
    {
        const BufferReturnMessage &message = msg.cast<BufferReturnMessage>();
        const size_t index = message.index;
        if (this->block_state == BLOCK_STATE_DONE) return;
        this->output_queues.push(index, message.buffer);
        this->handle_task(task_iface);
        return;
    }

    //self kick, call the handle task method
    if (msg.type() == typeid(SelfKickMessage))
    {
        this->handle_task(task_iface);
        return;
    }

    //clearly, this block is near death, hang on sparky
    if (msg.type() == typeid(CheckTokensMessage))
    {
        if (this->input_queues.all_ready() and not this->forecast_fail)
        {
            this->handle_task(task_iface);
        }
        else
        {
            this->mark_done(task_iface);
        }
        return;
    }

    //store the topology's thread group
    //erase any potentially old lingering threads
    //spawn a new thread if this block is a source
    if (msg.type() == typeid(SharedThreadGroup))
    {
        this->thread_group = msg.cast<SharedThreadGroup>();
        this->interruptible_thread.reset(); //erase old one
        if (task_iface.get_num_inputs() == 0) //its a source
        {
            this->interruptible_thread = boost::make_shared<InterruptibleThread>(
                this->thread_group, boost::bind(&ElementImpl::task_work, this)
            );
        }
        return;
    }

    //user changed some input settings like history or reserve reqs
    if (msg.type() == typeid(UpdateInputsMessage))
    {
        this->input_update(task_iface);
        return;
    }

    ASSERT(msg.type() == typeid(TopBlockMessage));

    //FIXME leave the marked done blocks done...
    //this helps QA tests to pass that re-use top block without diconnecting the old design
    if (this->block_state == BLOCK_STATE_DONE) return;

    const size_t num_inputs = task_iface.get_num_inputs();
    const size_t num_outputs = task_iface.get_num_outputs();

    //allocate output tokens and send them downstream
    if (msg.cast<TopBlockMessage>().what == TopBlockMessage::TOKEN_TIME)
    {
        for (size_t i = 0; i < num_inputs; i++)
        {
            this->input_tokens[i] = Token::make();
            task_iface.post_upstream(i, this->input_tokens[i]);

            //TODO, schedule this message as a pre-allocation message
            //tell the upstream about the input requirements
            BufferHintMessage message;
            message.history_bytes = this->input_history_items[i]*this->input_items_sizes[i];
            message.reserve_bytes = this->input_multiple_items[i];
            message.token = this->input_tokens[i];
            task_iface.post_upstream(i, message);

        }
        for (size_t i = 0; i < num_outputs; i++)
        {
            this->output_tokens[i] = Token::make();
            task_iface.post_downstream(i, this->output_tokens[i]);
        }
        this->token_pool.insert(msg.cast<TopBlockMessage>().token);
    }

    if (msg.cast<TopBlockMessage>().what == TopBlockMessage::ALLOCATE)
    {
        //causes initial processing kick-off for source blocks
        this->handle_allocation(task_iface);
    }

    if (msg.cast<TopBlockMessage>().what == TopBlockMessage::ACTIVE)
    {
        if (this->block_state != BLOCK_STATE_LIVE)
        {
            this->block_ptr->start();
        }
        this->block_state = BLOCK_STATE_LIVE;
        if (this->input_queues.all_ready() and this->output_queues.all_ready())
        {
            this->block.post_msg(SelfKickMessage());
        }
    }

    if (msg.cast<TopBlockMessage>().what == TopBlockMessage::INERT)
    {
        if (this->block_state != BLOCK_STATE_DONE)
        {
            this->block_ptr->stop();
        }
        this->mark_done(task_iface);
    }
}

void ElementImpl::topology_update(const tsbe::TaskInterface &task_iface)
{
    //std::cout << "topology_update in " << name << std::endl;

    const size_t num_inputs = task_iface.get_num_inputs();
    const size_t num_outputs = task_iface.get_num_outputs();

    //call check_topology on block before committing settings
    this->block_ptr->check_topology(num_inputs, num_outputs);

    //fill the item sizes from the IO signatures
    fill_item_sizes_from_sig(this->input_items_sizes, this->input_signature, num_inputs);
    fill_item_sizes_from_sig(this->output_items_sizes, this->output_signature, num_outputs);

    //resize and fill port properties
    resize_fill_back(this->input_history_items, num_inputs);
    resize_fill_back(this->output_multiple_items, num_outputs);
    resize_fill_grow(this->input_inline_enables, num_inputs, false);

    //resize the bytes consumed/produced
    resize_fill_grow(this->items_consumed, num_inputs, 0);
    resize_fill_grow(this->items_produced, num_outputs, 0);

    //resize all work buffers to match current connections
    this->work_input_items.resize(num_inputs);
    this->work_output_items.resize(num_outputs);
    this->work_ninput_items.resize(num_inputs);
    this->fcast_ninput_items.resize(num_inputs);
    this->input_items.resize(num_inputs);
    this->output_items.resize(num_outputs);
    this->consume_items.resize(num_inputs, 0);
    this->consume_called.resize(num_inputs, false);
    this->produce_items.resize(num_outputs, 0);
    this->input_queues.resize(num_inputs);
    this->output_queues.resize(num_outputs);
    this->forecast_enable = num_outputs != 0 and num_inputs != 0;

    this->input_tokens.resize(num_inputs);
    this->output_tokens.resize(num_outputs);
    this->output_allocation_hints.resize(num_outputs);

    //resize tags vector to match sizes
    this->input_tags_changed.resize(num_inputs);
    this->input_tags.resize(num_inputs);

    //a block looses all connections, allow it to free
    if (num_inputs == 0 and num_outputs == 0)
    {
        this->mark_done(task_iface);
    }

    this->topology_init = true;
    this->input_update(task_iface);
}

void ElementImpl::input_update(const tsbe::TaskInterface &task_iface)
{
    const size_t num_inputs = task_iface.get_num_inputs();
    const size_t num_outputs = task_iface.get_num_outputs();

    //impose input reserve requirements based on relative rate and output multiple
    resize_fill_grow(this->input_multiple_items, num_inputs, 1);
    for (size_t i = 0; i < num_inputs; i++)
    {
        //TODO, this is a little cheap, we only look at output multiple [0]
        const size_t multiple = (num_outputs)?this->output_multiple_items.front():1;
        if (this->enable_fixed_rate)
        {
            this->input_multiple_items[i] = size_t(std::ceil(multiple/this->relative_rate));
        }
        if (this->input_multiple_items[i] == 0) this->input_multiple_items[i] = 1;
    }

    //init the history comprehension on input queues
    this->input_queues.init(this->input_history_items, this->input_multiple_items, this->input_items_sizes);
}
