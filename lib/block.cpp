// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "element_impl.hpp"
#include <gras/block.hpp>
#include <boost/foreach.hpp>

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
    this->set_input_config(0, InputPortConfig());
    this->set_output_config(0, OutputPortConfig());
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

InputPortConfig Block::get_input_config(const size_t which_input) const
{
    return vector_get((*this)->block->input_configs, which_input);
}

void Block::set_input_config(const size_t which_input, const InputPortConfig &config)
{
    vector_set((*this)->block->input_configs, config, which_input);
    if ((*this)->block->topology_init)
        (*this)->block->Push(UpdateInputsMessage(), Theron::Address());
}

OutputPortConfig Block::get_output_config(const size_t which_output) const
{
    return vector_get((*this)->block->output_configs, which_output);
}

void Block::set_output_config(const size_t which_output, const OutputPortConfig &config)
{
    vector_set((*this)->block->output_configs, config, which_output);
    if ((*this)->block->topology_init)
        (*this)->block->Push(UpdateInputsMessage(), Theron::Address());
}

void Block::consume(const size_t which_input, const size_t num_items)
{
    ASSERT(long(num_items) >= 0); //sign bit set? you dont want a negative
    (*this)->block->consume(which_input, num_items);
}

void Block::produce(const size_t which_output, const size_t num_items)
{
    ASSERT(long(num_items) >= 0); //sign bit set? you dont want a negative
    (*this)->block->produce(which_output, num_items);
}

void Block::consume(const size_t num_items)
{
    const size_t num_inputs = (*this)->block->get_num_inputs();
    for (size_t i = 0; i < num_inputs; i++)
    {
        (*this)->block->consume(i, num_items);
    }
}

void Block::produce(const size_t num_items)
{
    const size_t num_outputs = (*this)->block->get_num_outputs();
    for (size_t o = 0; o < num_outputs; o++)
    {
        (*this)->block->produce(o, num_items);
    }
}

item_index_t Block::get_consumed(const size_t which_input)
{
    return (*this)->block->items_consumed[which_input];
}

item_index_t Block::get_produced(const size_t which_output)
{
    return (*this)->block->items_produced[which_output];
}

void Block::post_output_tag(const size_t which_output, const Tag &tag)
{
    (*this)->block->post_downstream(which_output, InputTagMessage(tag));
}

TagIter Block::get_input_tags(const size_t which_input)
{
    const std::vector<Tag> &input_tags = (*this)->block->input_tags[which_input];
    return TagIter(input_tags.begin(), input_tags.end());
}

void Block::erase_input_tags(const size_t which_input)
{
    (*this)->block->input_tags[which_input].clear();
}

Tag Block::pop_input_msg(const size_t which_input)
{
    std::vector<Tag> &input_tags = (*this)->block->input_tags[which_input];
    if (input_tags.empty()) return Tag();
    Tag t = input_tags.front();
    input_tags.erase(input_tags.begin());
    return t;
}

void Block::propagate_tags(const size_t i, const TagIter &iter)
{
    const size_t num_outputs = (*this)->block->get_num_outputs();
    for (size_t o = 0; o < num_outputs; o++)
    {
        BOOST_FOREACH(gras::Tag t, iter)
        {
            t.offset -= this->get_consumed(i);
            t.offset += this->get_produced(o);
            this->post_output_tag(o, t);
        }
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

const SBuffer &Block::get_input_buffer(const size_t which_input) const
{
    return (*this)->block->input_queues.front(which_input);
}

SBuffer Block::pop_output_buffer(const size_t which_output)
{
    SBuffer buff = (*this)->block->output_queues.front(which_output);
    (*this)->block->output_queues.pop(which_output);
    return buff;
}

void Block::post_output_buffer(const size_t which_output, const SBuffer &buffer)
{
    (*this)->block->produce_buffer(which_output, buffer);
}
