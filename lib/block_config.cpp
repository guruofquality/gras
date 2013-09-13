// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/block_config.hpp>

using namespace gras;

GlobalBlockConfig::GlobalBlockConfig(void)
{
    maximum_output_items = 0;
    buffer_affinity = -1;
    interruptible_work = false;
}

void GlobalBlockConfig::merge(const GlobalBlockConfig &config)
{
    //overwrite with config's max items only if maxium_items is not set (zero)
    if (this->maximum_output_items == 0)
    {
        this->maximum_output_items = config.maximum_output_items;
    }

    //overwrite with config's node affinity setting for buffers if not set
    if (this->buffer_affinity == -1)
    {
        this->buffer_affinity = config.buffer_affinity;
    }

    //overwrite with config's interruptable setting for work if not set
    if (this->interruptible_work == false)
    {
        this->interruptible_work = config.interruptible_work;
    }

    //overwrite with config's thread pool for actor if not set
    if (not this->thread_pool)
    {
        this->thread_pool = config.thread_pool;
    }
}

InputPortConfig::InputPortConfig(void)
{
    item_size = 1;
    reserve_items = 1;
    maximum_items = 0;
    inline_buffer = false;
    preload_items = 0;
}

OutputPortConfig::OutputPortConfig(void)
{
    item_size = 1;
    reserve_items = 1;
    maximum_items = 0;
}
