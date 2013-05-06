// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "element_impl.hpp"
#include <gras_impl/block_actor.hpp>

using namespace gras;

void Block::mark_output_fail(const size_t which_output)
{
    (*this)->block->output_fail(which_output);
}

void Block::mark_input_fail(const size_t which_input)
{
    (*this)->block->input_fail(which_input);
}

void BlockActor::input_fail(const size_t i)
{
    //input failed, accumulate and try again
    if (not this->input_queues.is_accumulated(i))
    {
        this->input_queues.accumulate(i);
        this->task_kicker();
        return;
    }

    //otherwise check for done, else wait for more
    if (this->inputs_done[i])
    {
        this->mark_done();
        return;
    }

    //check that the input is not already maxed
    if (this->input_queues.is_front_maximal(i))
    {
        throw std::runtime_error("input_fail called on maximum_items buffer in " + name);
    }

    //mark fail: not ready until a new buffer appears
    this->input_queues.fail(i);
}

void BlockActor::output_fail(const size_t i)
{
    SBuffer &buff = this->output_queues.front(i);

    //check that the input is not already maxed
    const size_t front_items = buff.length/this->output_configs[i].item_size;
    if (front_items >= this->output_configs[i].maximum_items)
    {
        throw std::runtime_error("output_fail called on maximum_items buffer in " + name);
    }

    //mark fail: not ready until a new buffer appears
    this->output_queues.fail(i);
}
