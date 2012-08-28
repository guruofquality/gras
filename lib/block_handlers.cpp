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

void ElementImpl::handle_port_msg(const size_t index, const tsbe::Wax &msg)
{
    if (msg.type() == typeid(Tag))
    {
        this->input_tags[index].push_back(msg.cast<Tag>());
        this->input_tags_changed[index] = true;
    }
}

template <typename V, typename T>
void resize_fill(V &v, const size_t new_len, const T &fill)
{
    if (v.size() >= new_len) return; //dont ever shrink it
    v.resize(new_len, fill);
}

template <typename V>
void resize_fill_back(V &v, const size_t new_len)
{
    if (v.empty()) v.push_back(0);
    resize_fill(v, new_len, v.back());
}

template <typename V, typename Sig>
void fill_item_sizes_from_sig(V &v, const Sig &s, const size_t size)
{
    v.resize(size);
    for (size_t i = 0; i < v.size(); i++)
    {
        v[i] = s->sizeof_stream_item(i);
    }
}

void ElementImpl::topology_update(const tsbe::TaskInterface &task_iface, const tsbe::Wax &state)
{
    const size_t num_inputs = task_iface.get_num_inputs();
    const size_t num_outputs = task_iface.get_num_outputs();

    //fill the item sizes from the IO signatures
    fill_item_sizes_from_sig(this->input_items_sizes, this->input_signature, num_inputs);
    fill_item_sizes_from_sig(this->output_items_sizes, this->output_signature, num_outputs);

    //resize and fill port properties
    resize_fill_back(this->input_history_items, num_inputs);
    resize_fill_back(this->output_multiple_items, num_outputs);

    //resize the bytes consumed/produced
    resize_fill(this->items_consumed, num_inputs, 0);
    resize_fill(this->items_produced, num_outputs, 0);

    //resize all work buffers to match current connections
    this->work_input_items.resize(num_inputs);
    this->work_output_items.resize(num_outputs);
    this->work_ninput_items.resize(num_inputs);
    this->input_items.resize(num_inputs);
    this->output_items.resize(num_outputs);
    this->consume_items.resize(num_inputs, 0);
    this->produce_items.resize(num_outputs, 0);
    this->input_buff_offsets.resize(num_inputs, 0);

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

    if (state.cast<TopBlockMessage>().what == TopBlockMessage::ACTIVE)
    {
        this->active = true;

        //causes initial processing kick-off for source blocks
        this->handle_allocation(task_iface);
    }
    if (state.cast<TopBlockMessage>().what == TopBlockMessage::INERT)
    {
        this->active = false;
    }
}
