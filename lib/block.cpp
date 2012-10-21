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
#include <gnuradio/block.hpp>

using namespace gnuradio;

InputPortConfig::InputPortConfig(void)
{
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
    this->set_fixed_rate(true);
    this->set_relative_rate(1.0);
    this->set_tag_propagation_policy(TPP_ALL_TO_ALL);
    this->set_interruptible_work(false);
    this->set_buffer_affinity(-1);
    (*this)->block->output_multiple_items = 1;
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
    (*this)->block->consume_items[which_input] += how_many_items;
    (*this)->block->consume_called[which_input] = true;
}

void Block::consume_each(const size_t how_many_items)
{
    for (size_t i = 0; i < (*this)->block->consume_items.size(); i++)
    {
        (*this)->block->consume_items[i] += how_many_items;
        (*this)->block->consume_called[i] = true;
    }
}

void Block::produce(const size_t which_output, const size_t how_many_items)
{
    (*this)->block->produce_items[which_output] += how_many_items;
}

void Block::set_fixed_rate(const bool fixed_rate)
{
    (*this)->block->enable_fixed_rate = fixed_rate;
}

bool Block::fixed_rate(void) const
{
    return (*this)->block->enable_fixed_rate;
}

void Block::set_output_multiple(const size_t multiple)
{
    (*this)->block->output_multiple_items = multiple;
    gnuradio::OutputPortConfig config = this->output_config();
    config.reserve_items = multiple;
    this->set_output_config(config);
}

size_t Block::output_multiple(void) const
{
    return (*this)->block->output_multiple_items;
}

void Block::set_relative_rate(double relative_rate)
{
    (*this)->block->relative_rate = relative_rate;
}

double Block::relative_rate(void) const
{
    return (*this)->block->relative_rate;
}

uint64_t Block::nitems_read(const size_t which_input)
{
    return (*this)->block->items_consumed[which_input];
}

uint64_t Block::nitems_written(const size_t which_output)
{
    return (*this)->block->items_produced[which_output];
}

Block::tag_propagation_policy_t Block::tag_propagation_policy(void)
{
    return (*this)->block->tag_prop_policy;
}

void Block::set_tag_propagation_policy(Block::tag_propagation_policy_t p)
{
    (*this)->block->tag_prop_policy = p;
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

void Block::forecast(int, std::vector<int> &)
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

bool Block::check_topology(int, int)
{
    return true;
}

void Block::set_buffer_affinity(const long affinity)
{
    (*this)->block->buffer_affinity = affinity;
}

void Block::set_interruptible_work(const bool enb)
{
    (*this)->block->interruptible_work = enb;
}
