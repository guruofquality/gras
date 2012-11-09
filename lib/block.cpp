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
#include <gras/block.hpp>

using namespace gras;

InputPortConfig::InputPortConfig(void)
{
    reserve_items = 1;
    maximum_items = 0;
    inline_buffer = false;
    lookahead_items = 0;
}

OutputPortConfig::OutputPortConfig(void)
{
    reserve_items = 1;
    maximum_items = 0;
}

Block::Block(void)
{
    //NOP
}

Block::Block(const std::string &name):
    Element(name)
{
    (*this)->block = boost::shared_ptr<BlockActor>(new BlockActor());
    (*this)->thread_pool = (*this)->block->thread_pool; //ref copy of pool
    (*this)->block->name = name; //for debug purposes

    //setup some state variables
    (*this)->block->topology_init = false;
    (*this)->block->block_ptr = this;
    (*this)->block->block_state = BlockActor::BLOCK_STATE_INIT;

    //call block methods to init stuff
    this->set_input_config(InputPortConfig());
    this->set_output_config(OutputPortConfig());
    this->set_interruptible_work(false);
    this->set_buffer_affinity(-1);
}

template <typename V, typename T>
void vector_set(V &v, const T &t, const size_t index)
{
    if (v.size() <= index)
    {
        v.resize(index+1, t);
    }
    v[index] = t;
}

template <typename V>
typename V::value_type vector_get(const V &v, const size_t index)
{
    if (v.size() <= index)
    {
        return v.front();
    }
    return v[index];
}

InputPortConfig Block::input_config(const size_t which_input) const
{
    return vector_get((*this)->block->input_configs, which_input);
}

void Block::set_input_config(const InputPortConfig &config, const size_t which_input)
{
    vector_set((*this)->block->input_configs, config, which_input);
    if ((*this)->block->topology_init)
        (*this)->block->Push(UpdateInputsMessage(), Theron::Address());
}

OutputPortConfig Block::output_config(const size_t which_output) const
{
    return vector_get((*this)->block->output_configs, which_output);
}

void Block::set_output_config(const OutputPortConfig &config, const size_t which_output)
{
    vector_set((*this)->block->output_configs, config, which_output);
    if ((*this)->block->topology_init)
        (*this)->block->Push(UpdateInputsMessage(), Theron::Address());
}

void Block::consume(const size_t which_input, const size_t how_many_items)
{
    (*this)->block->consume(which_input, how_many_items);
}

void Block::produce(const size_t which_output, const size_t how_many_items)
{
    (*this)->block->produce(which_output, how_many_items);
}

item_index_t Block::nitems_read(const size_t which_input)
{
    return (*this)->block->items_consumed[which_input];
}

item_index_t Block::nitems_written(const size_t which_output)
{
    return (*this)->block->items_produced[which_output];
}

void Block::post_output_tag(
    const size_t which_output,
    const Tag &tag
){
    (*this)->block->post_downstream(which_output, InputTagMessage(tag));
}

Block::TagIter Block::get_input_tags(
    const size_t which_input
){
    const std::vector<Tag> &input_tags = (*this)->block->input_tags[which_input];
    return boost::make_iterator_range(input_tags.begin(), input_tags.end());
}

void Block::propagate_tags(const size_t, const TagIter &)
{
    //NOP
}

bool Block::start(void)
{
    return true;
}

bool Block::stop(void)
{
    return true;
}

void Block::notify_topology(const size_t, const size_t)
{
    return;
}

void Block::set_buffer_affinity(const long affinity)
{
    (*this)->block->buffer_affinity = affinity;
}

void Block::set_interruptible_work(const bool enb)
{
    (*this)->block->interruptible_work = enb;
}

void Block::mark_output_fail(const size_t which_output)
{
    (*this)->block->output_fail(which_output);
}

void Block::mark_input_fail(const size_t which_input)
{
    (*this)->block->input_fail(which_input);
}

void Block::mark_done(void)
{
    (*this)->block->mark_done();
}
