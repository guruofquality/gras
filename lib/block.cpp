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
#include <gnuradio/block.hpp>
#include <boost/bind.hpp>

using namespace gnuradio;

Block::Block(void)
{
    //NOP
}

Block::Block(const std::string &name):
    Element(name)
{
    this->set_history(0);
    this->set_output_multiple(1);
    this->set_fixed_rate(true);
    this->set_relative_rate(1.0);
    this->set_tag_propagation_policy(TPP_ALL_TO_ALL);

    tsbe::BlockConfig config;
    config.input_callback = boost::bind(&ElementImpl::handle_input_msg, this->get(), _1, _2, _3);
    config.output_callback = boost::bind(&ElementImpl::handle_output_msg, this->get(), _1, _2, _3);
    config.block_callback = boost::bind(&ElementImpl::handle_block_msg, this->get(), _1, _2);
    config.changed_callback = boost::bind(&ElementImpl::topology_update, this->get(), _1);
    (*this)->block = tsbe::Block(config);

    (*this)->block_ptr = this;
    (*this)->hint = 0;
    (*this)->block_state = ElementImpl::BLOCK_STATE_INIT;
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

size_t Block::history(const size_t which_input) const
{
    return vector_get((*this)->input_history_items, which_input);
}

void Block::set_history(const size_t history, const size_t which_input)
{
    vector_set((*this)->input_history_items, history, which_input);
}

size_t Block::output_multiple(const size_t which_output) const
{
    return vector_get((*this)->output_multiple_items, which_output);
}

void Block::set_output_multiple(const size_t multiple, const size_t which_output)
{
    vector_set((*this)->output_multiple_items, multiple, which_output);
}

void Block::consume(const size_t which_input, const size_t how_many_items)
{
    (*this)->consume_items[which_input] = how_many_items;
}

void Block::consume_each(const size_t how_many_items)
{
    for (size_t i = 0; i < (*this)->consume_items.size(); i++)
    {
        (*this)->consume_items[i] = how_many_items;
    }
}

void Block::produce(const size_t which_output, const size_t how_many_items)
{
    (*this)->produce_items[which_output] = how_many_items;
}

void Block::set_fixed_rate(const bool fixed_rate)
{
    (*this)->enable_fixed_rate = fixed_rate;
}

void Block::set_relative_rate(double relative_rate)
{
    (*this)->relative_rate = relative_rate;
}

double Block::relative_rate(void) const
{
    return (*this)->relative_rate;
}

uint64_t Block::nitems_read(const size_t which_input)
{
    return (*this)->items_consumed[which_input];
}

uint64_t Block::nitems_written(const size_t which_output)
{
    return (*this)->items_produced[which_output];
}

Block::tag_propagation_policy_t Block::tag_propagation_policy(void)
{
    return (*this)->tag_prop_policy;
}

void Block::set_tag_propagation_policy(Block::tag_propagation_policy_t p)
{
    (*this)->tag_prop_policy = p;
}

void Block::add_item_tag(
    const size_t which_output,
    const Tag &tag
){
    (*this)->output_tags[which_output].push_back(tag);
}

void Block::add_item_tag(
    const size_t which_output,
    uint64_t abs_offset,
    const pmt::pmt_t &key,
    const pmt::pmt_t &value,
    const pmt::pmt_t &srcid
){
    Tag t;
    t.offset = abs_offset;
    t.key = key;
    t.value = value;
    t.srcid = srcid;
    this->add_item_tag(which_output, t);
}

void Block::get_tags_in_range(
    std::vector<Tag> &tags,
    const size_t which_input,
    uint64_t abs_start,
    uint64_t abs_end
){
    const std::vector<Tag> &input_tags = (*this)->input_tags[which_input];
    tags.clear();
    for (size_t i = 0; i < input_tags.size(); i++)
    {
        if (input_tags[i].offset >= abs_start and input_tags[i].offset <= abs_end)
        {
            tags.push_back(input_tags[i]);
        }
    }
}

void Block::get_tags_in_range(
    std::vector<Tag> &tags,
    const size_t which_input,
    uint64_t abs_start,
    uint64_t abs_end,
    const pmt::pmt_t &key
){
    const std::vector<Tag> &input_tags = (*this)->input_tags[which_input];
    tags.clear();
    for (size_t i = 0; i < input_tags.size(); i++)
    {
        if (
            input_tags[i].offset >= abs_start and
            input_tags[i].offset <= abs_end and
            pmt::pmt_equal(input_tags[i].key, key)
        ){
            tags.push_back(input_tags[i]);
        }
    }
}

void Block::forecast(
    int noutput_items,
    std::vector<int> &ninput_items_required
){
    for (size_t i = 0; i < ninput_items_required.size(); i++)
    {
        ninput_items_required[i] =
            (*this)->input_history_items[i] +
            myulround((noutput_items/(*this)->relative_rate));
    }
}

bool Block::start(void)
{
    return true;
}

bool Block::stop(void)
{
    return true;
}
