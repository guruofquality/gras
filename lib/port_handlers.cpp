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

using namespace gnuradio;

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

void ElementImpl::handle_input_msg(
    const tsbe::TaskInterface &handle,
    const size_t index,
    const tsbe::Wax &msg
){
    if (MESSAGE) std::cerr << "handle_input_msg (" << msg.type().name() << ") " << name << std::endl;

    //handle incoming stream buffer, push into the queue
    if (msg.type() == typeid(SBuffer))
    {
        if (this->block_state == BLOCK_STATE_DONE) return;
        this->input_queues.push(index, msg.cast<SBuffer>());
        this->handle_task(handle);
        return;
    }

    //handle incoming stream tag, push into the tag storage
    if (msg.type() == typeid(Tag))
    {
        this->input_tags[index].push_back(msg.cast<Tag>());
        this->input_tags_changed[index] = true;
        return;
    }

    //store the token of the upstream producer
    if (msg.type() == typeid(Token))
    {
        this->token_pool.insert(msg.cast<Token>());
        return;
    }

    //an upstream block declared itself done, recheck the token
    if (msg.type() == typeid(CheckTokensMessage))
    {
        if (this->input_queues.empty(index) and this->input_tokens[index].unique())
        {
            this->mark_done(handle);
        }
        return;
    }

    //handle the upstream block allocation request
    if (msg.type() == typeid(InputAllocatorMessage))
    {
        InputAllocatorMessage message;
        message.token = block_ptr->input_buffer_allocator(
            index,
            msg.cast<InputAllocatorMessage>().token,
            msg.cast<InputAllocatorMessage>().recommend_length
        );
        if (message.token) handle.post_upstream(index, message);
        return;
    }

    ASSERT(false);
}

void ElementImpl::handle_output_msg(
    const tsbe::TaskInterface &handle,
    const size_t index,
    const tsbe::Wax &msg
){
    if (MESSAGE) std::cerr << "handle_output_msg (" << msg.type().name() << ") " << name << std::endl;

    //store the token of the downstream consumer
    if (msg.type() == typeid(Token))
    {
        this->token_pool.insert(msg.cast<Token>());
        return;
    }

    //a downstream block has declared itself done, recheck the token
    if (msg.type() == typeid(CheckTokensMessage))
    {
        if (this->output_tokens[index].unique())
        {
            this->mark_done(handle);
        }
        return;
    }

    //update the buffer allocation hint
    if (msg.type() == typeid(BufferHintMessage))
    {
        //this->output_allocation_hints.resize(std::max(output_allocation_hints.size(), index+1));
        const BufferHintMessage new_hint = msg.cast<BufferHintMessage>();

        //remove any old hints with expired token
        //remove any older hints with matching token
        std::vector<BufferHintMessage> hints;
        BOOST_FOREACH(const BufferHintMessage &hint, this->output_allocation_hints[index])
        {
            if (hint.token.expired()) continue;
            if (hint.token.lock() == new_hint.token.lock()) continue;
            hints.push_back(hint);
        }

        //store the new hint as well
        hints.push_back(new_hint);

        this->output_allocation_hints[index] = hints;
        return;
    }

    //return of a positive downstream allocation
    //reset the token, and clear old output buffers
    //the new token from the downstream is installed
    if (msg.type() == typeid(InputAllocatorMessage))
    {
        this->output_buffer_tokens[index].reset();
        this->output_queues.flush(index);
        this->output_buffer_tokens[index] = msg.cast<InputAllocatorMessage>().token;
    }

    ASSERT(false);
}
