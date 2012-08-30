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
#include "vec_utils_impl.hpp"

using namespace gnuradio;

void ElementImpl::topology_update(const tsbe::TaskInterface &task_iface, const tsbe::Wax &state)
{
    if (state.type() == typeid(BufferReturnMessage))
    {
        const BufferReturnMessage &message = state.cast<BufferReturnMessage>();
        this->handle_output_msg(task_iface, message.index, message.buffer);
        return;
    }

    const size_t num_inputs = task_iface.get_num_inputs();
    const size_t num_outputs = task_iface.get_num_outputs();

    //fill the item sizes from the IO signatures
    fill_item_sizes_from_sig(this->input_items_sizes, this->input_signature, num_inputs);
    fill_item_sizes_from_sig(this->output_items_sizes, this->output_signature, num_outputs);

    //resize and fill port properties
    resize_fill_back(this->input_history_items, num_inputs);
    resize_fill_back(this->output_multiple_items, num_outputs);

    //resize the bytes consumed/produced
    resize_fill_grow(this->items_consumed, num_inputs, 0);
    resize_fill_grow(this->items_produced, num_outputs, 0);

    //resize all work buffers to match current connections
    this->work_input_items.resize(num_inputs);
    this->work_output_items.resize(num_outputs);
    this->work_ninput_items.resize(num_inputs);
    this->input_items.resize(num_inputs);
    this->output_items.resize(num_outputs);
    this->consume_items.resize(num_inputs, 0);
    this->produce_items.resize(num_outputs, 0);
    this->input_buff_offsets.resize(num_inputs, 0);
    this->input_queues.resize(num_inputs);
    this->output_queues.resize(num_outputs);
    this->inputs_ready.resize(num_inputs);
    this->outputs_ready.resize(num_outputs);

    //resize tags vector to match sizes
    this->input_tags_changed.resize(num_inputs);
    this->input_tags.resize(num_inputs);
    this->output_tags.resize(num_outputs);

    //resize and clear that initial history
    this->history_buffs.resize(num_inputs);
    for (size_t i = 0; i < num_inputs; i++)
    {
        tsbe::Buffer &buff = this->history_buffs[i];
        const size_t num_bytes = this->input_items_sizes[i]*this->input_history_items[i];
        if (not buff or buff.get_length() != num_bytes)
        {
            tsbe::BufferConfig config;
            config.memory = NULL;
            config.length = num_bytes;
            buff = tsbe::Buffer(config);
        }
    }

    //allocate output tokens and send them downstream
    if (state.cast<TopBlockMessage>().what == TopBlockMessage::ACTIVE)
    {
        this->input_tokens.resize(num_inputs);
        for (size_t i = 0; i < num_inputs; i++)
        {
            this->input_tokens[i] = Token::make();
            task_iface.post_upstream(i, this->input_tokens[i]);
        }
        this->output_tokens.resize(num_outputs);
        for (size_t i = 0; i < num_outputs; i++)
        {
            this->output_tokens[i] = Token::make();
            task_iface.post_downstream(i, this->output_tokens[i]);
        }
    }

    if (state.cast<TopBlockMessage>().what == TopBlockMessage::ACTIVE)
    {
        this->active = true;
        this->done = false;
        this->token_pool.insert(state.cast<TopBlockMessage>().token);

        //causes initial processing kick-off for source blocks
        this->handle_allocation(task_iface);
    }
    if (state.cast<TopBlockMessage>().what == TopBlockMessage::INERT)
    {
        this->mark_done(task_iface);
    }

    //TODO: think more about this:
    if (num_inputs == 0 and num_outputs == 0)
    {
        this->mark_done(task_iface);
    }

    //TODO: generate a message to handle task in a loop for a while
    //we may need to call it into exhaustion to be correct
    //but dont call it from update, let the settings above sink in
    this->handle_task(task_iface);
    this->handle_task(task_iface);
}
