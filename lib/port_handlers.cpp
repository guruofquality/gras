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

using namespace gnuradio;

void ElementImpl::handle_input_msg(
    const tsbe::TaskInterface &handle,
    const size_t index,
    const tsbe::Wax &msg
){
    //std::cout << "handle_input_msg in " << name << std::endl;

    //handle incoming stream buffer, push into the queue
    if (msg.type() == typeid(tsbe::Buffer))
    {
        if (this->block_state == BLOCK_STATE_DONE) return;
        this->input_queues.push(index, msg.cast<tsbe::Buffer>());
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
}

void ElementImpl::handle_output_msg(
    const tsbe::TaskInterface &handle,
    const size_t index,
    const tsbe::Wax &msg
){
    //std::cout << "handle_output_msg in " << name << std::endl;

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
}
