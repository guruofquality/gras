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

#include <gras_impl/block_actor.hpp>
#include <boost/foreach.hpp>

using namespace gnuradio;


void BlockActor::handle_output_buffer(const OutputBufferMessage &message, const Theron::Address)
{
    MESSAGE_TRACER();
    const size_t index = message.index;

    //a buffer has returned from the downstream
    //(all interested consumers have finished with it)
    if (this->block_state == BLOCK_STATE_DONE) return;
    this->output_queues.push(index, message.buffer);
    this->handle_task();
}

void BlockActor::handle_output_token(const OutputTokenMessage &message, const Theron::Address)
{
    MESSAGE_TRACER();
    ASSERT(message.index < this->get_num_outputs());

    //store the token of the downstream consumer
    this->token_pool.insert(message.token);
}

void BlockActor::handle_output_check(const OutputCheckMessage &message, const Theron::Address)
{
    MESSAGE_TRACER();
    const size_t index = message.index;

    //a downstream block has declared itself done, recheck the token
    this->outputs_done.set(index, this->output_tokens[index].unique());
    if (this->outputs_done.all()) //no downstream subscribers?
    {
        this->mark_done();
    }
}

void BlockActor::handle_output_hint(const OutputHintMessage &message, const Theron::Address)
{
    MESSAGE_TRACER();
    const size_t index = message.index;

    //update the buffer allocation hint
    //this->output_allocation_hints.resize(std::max(output_allocation_hints.size(), index+1));

    //remove any old hints with expired token
    //remove any older hints with matching token
    std::vector<OutputHintMessage> hints;
    BOOST_FOREACH(const OutputHintMessage &hint, this->output_allocation_hints[index])
    {
        if (hint.token.expired()) continue;
        if (hint.token.lock() == message.token.lock()) continue;
        hints.push_back(hint);
    }

    //store the new hint as well
    hints.push_back(message);

    this->output_allocation_hints[index] = hints;
}

void BlockActor::handle_output_alloc(const OutputAllocMessage &message, const Theron::Address)
{
    MESSAGE_TRACER();
    const size_t index = message.index;

    //return of a positive downstream allocation
    //reset the token, and clear old output buffers
    //the new token from the downstream is installed
    this->output_buffer_tokens[index].reset();
    this->output_queues.flush(index);
    this->output_buffer_tokens[index] = message.token;
}
