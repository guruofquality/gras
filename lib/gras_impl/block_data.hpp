// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_LIBGRAS_IMPL_BLOCK_DATA_HPP
#define INCLUDED_LIBGRAS_IMPL_BLOCK_DATA_HPP

#include <gras/block.hpp>
#include <gras_impl/debug.hpp>
#include <gras_impl/bitset.hpp>
#include <gras_impl/token.hpp>
#include <gras_impl/stats.hpp>
#include <gras_impl/output_buffer_queues.hpp>
#include <gras_impl/input_buffer_queues.hpp>
#include <gras_impl/interruptible_thread.hpp>
#include <vector>
#include <set>
#include <map>

namespace gras
{

enum BlockState
{
    BLOCK_STATE_INIT,
    BLOCK_STATE_LIVE,
    BLOCK_STATE_DONE,
};

struct BlockData
{
    //block pointer to call into parent
    Block *block;

    //per port properties
    std::vector<InputPortConfig> input_configs;
    std::vector<OutputPortConfig> output_configs;

    //work buffers for the new work interface
    Block::InputItems input_items;
    Block::OutputItems output_items;

    //track the subscriber counts
    std::vector<Token> input_tokens;
    std::vector<Token> output_tokens;
    BitSet inputs_done;
    BitSet outputs_done;
    std::set<Token> token_pool;
    boost::system_time first_input_done_time;

    //buffer queues and ready conditions
    InputBufferQueues input_queues;
    OutputBufferQueues output_queues;
    BitSet inputs_available;
    std::vector<time_ticks_t> time_input_not_ready;
    std::vector<time_ticks_t> time_output_not_ready;

    //tag and msg tracking
    std::vector<bool> input_tags_changed;
    std::vector<std::vector<Tag> > input_tags;
    std::vector<size_t> num_input_msgs_read;
    std::vector<size_t> num_input_items_read;
    std::vector<size_t> num_output_items_read;
    std::vector<size_t> total_items_consumed;
    std::vector<size_t> total_items_produced;
    std::vector<std::vector<PMCC> > input_msgs;

    //interruptible thread stuff
    SharedThreadGroup thread_group;
    boost::shared_ptr<InterruptibleThread> interruptible_thread;

    //is the fg running?
    BlockState block_state;

    std::vector<std::vector<OutputHintMessage> > output_allocation_hints;

    BlockStats stats;
};

} //namespace gras

#endif /*INCLUDED_LIBGRAS_IMPL_BLOCK_DATA_HPP*/
