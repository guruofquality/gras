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
    //overwrite with global config only if maxium_items is not set (zero)
    if (this->maximum_output_items == 0)
    {
        this->maximum_output_items = config.maximum_output_items;
    }

    //overwrite with global node affinity setting for buffers if not set
    if (this->buffer_affinity == -1)
    {
        this->buffer_affinity = config.buffer_affinity;
    }

    //overwrite with global interruptable setting for work if not set
    if (this->interruptible_work == false)
    {
        this->interruptible_work = config.interruptible_work;
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
