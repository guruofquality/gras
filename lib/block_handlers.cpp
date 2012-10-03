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
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>

using namespace gnuradio;


void BlockActor::handle_top_active(
    const TopActiveMessage &,
    const Theron::Address from
){
    MESSAGE_TRACER();

    if (this->block_state == BLOCK_STATE_DONE)
    {
        this->Send(0, from); //ACK
        return;
    }

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

    if (this->block_state == BLOCK_STATE_DONE)
    {
        this->Send(0, from); //ACK
        return;
    }

    //create input tokens and send allocation hints
    for (size_t i = 0; i < this->get_num_inputs(); i++)
    {
        this->input_tokens[i] = Token::make();
        this->inputs_done.reset(i);
        OutputTokenMessage token_msg;
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
        this->outputs_done.reset(i);
        InputTokenMessage token_msg;
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
