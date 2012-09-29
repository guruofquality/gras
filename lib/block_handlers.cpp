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


void BlockActor::handle_top_active(
    const TopActiveMessage &,
    const Theron::Address from
){
    MESSAGE_TRACER();

    if (this->block_state != BLOCK_STATE_LIVE)
    {
        this->block_ptr->start();
    }
    this->block_state = BLOCK_STATE_LIVE;
    if (this->input_queues.all_ready() and this->output_queues.all_ready())
    {
        this->Push(SelfKickMessage(), Theron::Address());
    }

    this->Send(0, from); //ACK
}

void BlockActor::handle_top_inert(
    const TopInertMessage &,
    const Theron::Address from
){
    MESSAGE_TRACER();

    if (this->block_state != BLOCK_STATE_DONE)
    {
        this->block_ptr->stop();
    }
    this->mark_done();

    this->Send(0, from); //ACK
}

void BlockActor::handle_top_token(
    const TopTokenMessage &message,
    const Theron::Address from
){
    MESSAGE_TRACER();

    //create input tokens and send allocation hints
    for (size_t i = 0; i < this->get_num_inputs(); i++)
    {
        this->input_tokens[i] = Token::make();
        InputTokenMessage token_msg;
        token_msg.token = this->input_tokens[i];
        this->post_upstream(i, token_msg);

        //TODO, schedule this message as a pre-allocation message
        //tell the upstream about the input requirements
        OutputHintMessage output_hints;
        output_hints.history_bytes = this->input_history_items[i]*this->input_items_sizes[i];
        output_hints.reserve_bytes = this->input_multiple_items[i];
        output_hints.token = this->input_tokens[i];
        this->post_upstream(i, output_hints);

    }

    //create output token
    for (size_t i = 0; i < this->get_num_outputs(); i++)
    {
        this->output_tokens[i] = Token::make();
        OutputTokenMessage token_msg;
        token_msg.token = this->output_tokens[i];
        this->post_downstream(i, token_msg);
    }

    //store a token to the top level topology
    this->token_pool.insert(message.token);

    this->Send(0, from); //ACK
}

void BlockActor::handle_top_hint(
    const TopHintMessage &message,
    const Theron::Address from
){
    MESSAGE_TRACER();

    this->hint = message.hint;

    this->Send(0, from); //ACK
}

void BlockActor::handle_top_thread_group(
    const SharedThreadGroup &message,
    const Theron::Address from
){
    MESSAGE_TRACER();

    //store the topology's thread group
    //erase any potentially old lingering threads
    //spawn a new thread if this block is a source
    this->thread_group = message;
    this->interruptible_thread.reset(); //erase old one
    if (this->get_num_inputs() == 0) //its a source
    {
        this->interruptible_thread = boost::make_shared<InterruptibleThread>(
            this->thread_group, boost::bind(&BlockActor::task_work, this)
        );
    }

    this->Send(0, from); //ACK
}

void BlockActor::handle_self_kick(
    const SelfKickMessage &,
    const Theron::Address
){
    MESSAGE_TRACER();
    this->handle_task();
}

void BlockActor::handle_check_tokens(
    const CheckTokensMessage &,
    const Theron::Address
){
    MESSAGE_TRACER();
    if (this->input_queues.all_ready() and not this->forecast_fail)
    {
        this->handle_task();
    }
    else
    {
        this->mark_done();
    }
}

void BlockActor::handle_topology(
    const Apology::WorkerTopologyMessage &,
    const Theron::Address
){
    MESSAGE_TRACER();

    const size_t num_inputs = this->get_num_inputs();
    const size_t num_outputs = this->get_num_outputs();

    //call check_topology on block before committing settings
    this->block_ptr->check_topology(num_inputs, num_outputs);

    //fill the item sizes from the IO signatures
    fill_item_sizes_from_sig(this->input_items_sizes, (*block_ptr)->input_signature, num_inputs);
    fill_item_sizes_from_sig(this->output_items_sizes, (*block_ptr)->output_signature, num_outputs);

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
        this->mark_done();
    }

    this->topology_init = true;
    this->handle_update_inputs(UpdateInputsMessage(), Theron::Address());
}

void BlockActor::handle_update_inputs(
    const UpdateInputsMessage &,
    const Theron::Address
){
    const size_t num_inputs = this->get_num_inputs();
    const size_t num_outputs = this->get_num_outputs();

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
