// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/block_config.hpp>

using namespace gras;

GlobalBlockConfig::GlobalBlockConfig(void)
{
    maximum_output_items = 0;
    buffer_affinity = -1;
    interruptible_work = false;
}

InputPortConfig::InputPortConfig(void)
{
    item_size = 1;
    reserve_items = 1;
    maximum_items = 0;
    inline_buffer = false;
    preload_items = 0;
    force_done = true;
}

OutputPortConfig::OutputPortConfig(void)
{
    item_size = 1;
    reserve_items = 1;
    maximum_items = 0;
}
