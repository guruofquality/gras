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

#include "element_impl.hpp"
#include <gras_impl/block_actor.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/math/common_factor.hpp>

using namespace gnuradio;

const size_t AT_LEAST_DEFAULT_ITEMS = 1 << 13;
const size_t AHH_TOO_MANY_BYTES = 1 << 20; //TODO
const size_t THIS_MANY_BUFFERS = 32;
const double EDGE_CASE_MITIGATION = 8.0; //edge case mitigation constant

//TODO will need more complicated later

void BlockActor::buffer_returner(const size_t index, SBuffer &buffer)
{
    //reset offset and length
    buffer.offset = 0;
    buffer.length = 0;

    OutputBufferMessage message;
    message.index = index;
    message.buffer = buffer;
    this->Push(message, Theron::Address());
}

static size_t recommend_length(
    const std::vector<OutputHintMessage> &hints,
    const size_t output_multiple_bytes,
    const size_t at_least_bytes
){
    //step 1) find the LCM of all reserves to create a super-reserve
    size_t lcm_bytes = output_multiple_bytes;
    BOOST_FOREACH(const OutputHintMessage &hint, hints)
    {
        lcm_bytes = boost::math::lcm(lcm_bytes, hint.reserve_bytes);
    }

    //step 2) N x super reserve to minimize history edge case
    size_t Nlcm_bytes = lcm_bytes;
    BOOST_FOREACH(const OutputHintMessage &hint, hints)
    {
        while (hint.history_bytes*EDGE_CASE_MITIGATION > Nlcm_bytes)
        {
            Nlcm_bytes += lcm_bytes;
        }
    }
    while (at_least_bytes > Nlcm_bytes)
    {
        Nlcm_bytes += lcm_bytes;
    }

    return std::min(Nlcm_bytes, AHH_TOO_MANY_BYTES);
}

void BlockActor::handle_top_alloc(const TopAllocMessage &, const Theron::Address from)
{
    MESSAGE_TRACER();

    if (this->block_state == BLOCK_STATE_DONE)
    {
        this->Send(0, from); //ACK
        return;
    }

    //allocate output buffers which will also wake up the task
    const size_t num_outputs = this->get_num_outputs();
    this->output_buffer_tokens.resize(num_outputs);
    for (size_t i = 0; i < num_outputs; i++)
    {
        size_t at_least_items = this->hint;
        if (at_least_items == 0) at_least_items = AT_LEAST_DEFAULT_ITEMS;

        const size_t bytes = recommend_length(
            this->output_allocation_hints[i],
            this->output_multiple_items[i]*this->output_items_sizes[i],
            at_least_items*this->output_items_sizes[i]
        );

        SBufferDeleter deleter = boost::bind(&BlockActor::buffer_returner, this, i, _1);
        SBufferToken token = SBufferToken(new SBufferDeleter(deleter));

        this->output_buffer_tokens[i] = block_ptr->output_buffer_allocator(i, token, bytes);

        InputAllocMessage message;
        message.token = SBufferToken(new SBufferDeleter(deleter));
        message.recommend_length = bytes;
        this->post_downstream(i, message);
    }

    this->Send(0, from); //ACK
}

SBufferToken Block::output_buffer_allocator(
    const size_t,
    const SBufferToken &token,
    const size_t recommend_length
){
    for (size_t j = 0; j < THIS_MANY_BUFFERS; j++)
    {
        SBufferConfig config;
        config.memory = NULL;
        config.length = recommend_length;
        config.affinity = (*this)->block->buffer_affinity;
        config.token = token;
        SBuffer buff(config);
        std::memset(buff.get_actual_memory(), 0, buff.get_actual_length());
        //buffer derefs here and the token messages it back to the block
    }
    return token;
}

SBufferToken Block::input_buffer_allocator(
    const size_t,
    const SBufferToken &,
    const size_t
){
    return SBufferToken(); //null
}
