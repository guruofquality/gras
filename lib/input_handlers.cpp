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

void BlockActor::handle_input_tag(const InputTagMessage &message, const Theron::Address)
{
    MESSAGE_TRACER();
    const size_t index = message.index;

    //handle incoming stream tag, push into the tag storage
    this->input_tags[index].push_back(message.tag);
    this->input_tags_changed[index] = true;
}

void BlockActor::handle_input_buffer(const InputBufferMessage &message, const Theron::Address)
{
    MESSAGE_TRACER();
    const size_t index = message.index;

    //handle incoming stream buffer, push into the queue
    if (this->block_state == BLOCK_STATE_DONE) return;
    this->input_queues.push(index, message.buffer);
    this->handle_task();
}

void BlockActor::handle_input_token(const InputTokenMessage &message, const Theron::Address)
{
    MESSAGE_TRACER();
    //const size_t index = message.index;

    //store the token of the upstream producer
    this->token_pool.insert(message.token);
}

void BlockActor::handle_input_check(const InputCheckMessage &message, const Theron::Address)
{
    MESSAGE_TRACER();
    const size_t index = message.index;

    //an upstream block declared itself done, recheck the token
    if (this->input_queues.empty(index) and this->input_tokens[index].unique())
    {
        this->mark_done();
    }
}

void BlockActor::handle_input_alloc(const InputAllocMessage &message, const Theron::Address)
{
    MESSAGE_TRACER();
    const size_t index = message.index;

    //handle the upstream block allocation request
    OutputAllocMessage new_msg;
    new_msg.token = block_ptr->input_buffer_allocator(
        index, message.token, message.recommend_length
    );
    if (new_msg.token) this->post_upstream(index, new_msg);
}
