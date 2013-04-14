// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "element_impl.hpp"
#include <gras/block.hpp>
#include <boost/foreach.hpp>

using namespace gras;

void Block::post_output_tag(const size_t which_output, const Tag &tag)
{
    (*this)->block->stats.tags_produced[which_output]++;
    (*this)->block->post_downstream(which_output, InputTagMessage(tag));
}

void Block::post_output_msg(const size_t which_output, const PMCC &msg)
{
    (*this)->block->stats.msgs_produced[which_output]++;
    (*this)->block->post_downstream(which_output, InputMsgMessage(msg));
}

TagIter Block::get_input_tags(const size_t which_input)
{
    const std::vector<Tag> &input_tags = (*this)->block->input_tags[which_input];
    return TagIter(input_tags.begin(), input_tags.end());
}

PMCC Block::pop_input_msg(const size_t which_input)
{
    std::vector<PMCC> &input_msgs = (*this)->block->input_msgs[which_input];
    size_t &num_read = (*this)->block->num_input_msgs_read[which_input];
    if (num_read >= input_msgs.size()) return PMCC();
    PMCC p = input_msgs[num_read++];
    (*this)->block->stats.msgs_consumed[which_input]++;
    return p;
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
