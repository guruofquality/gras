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
#include <gras_impl/messages.hpp>
#include <boost/bind.hpp>

using namespace gnuradio;

//TODO will need more complicated later


void ElementImpl::buffer_returner(const size_t index, SBuffer &buffer)
{
    //reset offset and length
    buffer.offset = 0;
    buffer.length = 0;

    BufferReturnMessage message;
    message.index = index;
    message.buffer = buffer;
    this->block.post_msg(message);
}

void ElementImpl::handle_allocation(const tsbe::TaskInterface &task_iface)
{
    //allocate output buffers which will also wake up the task
    const size_t num_outputs = task_iface.get_num_outputs();
    this->output_buffer_tokens.resize(num_outputs);
    for (size_t i = 0; i < num_outputs; i++)
    {
        size_t items = this->hint;
        if (items == 0) items = 2048;
        items = std::max(items, this->output_multiple_items[i]);

        const size_t bytes = items * this->output_items_sizes[i];

        SBufferDeleter deleter = boost::bind(&ElementImpl::buffer_returner, this, i, _1);
        this->output_buffer_tokens[i] = SBufferToken(new SBufferDeleter(deleter));

        for (size_t j = 0; j < 8; j++)
        {
            SBufferConfig config;
            config.memory = NULL;
            config.length = bytes;
            config.token = this->output_buffer_tokens[i];
            SBuffer buff(config);
            //buffer derefs here and the token messages it back to the block
        }
    }
}
