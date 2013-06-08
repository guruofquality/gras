// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "element_impl.hpp"
#include <gras_impl/block_actor.hpp>

using namespace gras;

void Block::mark_output_fail(const size_t which_output)
{
    (*this)->block_actor->output_fail(which_output);
}

void Block::mark_input_fail(const size_t which_input)
{
    (*this)->block_actor->input_fail(which_input);
}

void BlockActor::input_fail(const size_t i)
{
    //input failed, accumulate and try again
    if (not data->input_queues.is_accumulated(i))
    {
        data->input_queues.accumulate(i);
        this->task_kicker();
        return;
    }

    //otherwise check for done, else wait for more
    if (data->inputs_done[i])
    {
        this->mark_done();
        return;
    }

    //check that the input is not already maxed
    if (not data->input_queues.empty(i) and data->input_queues.is_front_maximal(i))
    {
        throw std::runtime_error("input_fail called on maximum_items buffer in " + name);
    }

    //mark fail: not ready until a new buffer appears
    data->input_queues.fail(i);
}

void BlockActor::output_fail(const size_t i)
{
    SBuffer &buff = data->output_queues.front(i);

    //check that the input is not already maxed
    const size_t front_items = buff.length/data->output_configs[i].item_size;
    if (front_items >= data->output_configs[i].maximum_items)
    {
        throw std::runtime_error("output_fail called on maximum_items buffer in " + name);
    }

    //mark fail: not ready until a new buffer appears
    data->output_queues.fail(i);
}
