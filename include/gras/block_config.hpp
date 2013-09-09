// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_BLOCK_CONFIG_HPP
#define INCLUDED_GRAS_BLOCK_CONFIG_HPP

#include <gras/gras.hpp>
#include <cstddef>

namespace gras
{

//! Configuration parameters for a block
struct GRAS_API GlobalBlockConfig
{
    GlobalBlockConfig(void);

    /*!
     * Merge the settings from another config.
     * Non-defaults on this config stay,
     * defaults will be overwritten.
     */
    void merge(const GlobalBlockConfig &config);

    /*!
     * Constrain the maximum number of items that
     * work can be called with for all output ports.
     *
     * Default = 0 aka disabled.
     */
    size_t maximum_output_items;

    /*!
     * Set the global memory node affinity.
     * Blocks that have not been explicitly set,
     * will take on this new buffer_affinity.
     *
     * This param affects how buffers are allocated.
     * By default memory is allocated by malloc.
     * When the affinity is set, virtual memory
     * will be locked to a physical CPU/memory node.
     *
     * Default = -1 aka no affinity.
     */
    long buffer_affinity;

    /*!
     * True if the work call should be interruptible by stop().
     * Some work implementations block with the expectation of
     * getting a boost thread interrupt in a blocking call.
     * If this is the case, this parameter should be set true.
     * By default, work implementations are not interruptible.
     *
     * Default = false.
     */
    bool interruptible_work;
};

//! Configuration parameters for an input port
struct GRAS_API InputPortConfig
{
    InputPortConfig(void);

    //! The size of an item in bytes
    size_t item_size;

    /*!
     * Set an input reserve requirement such that work is called
     * with an input buffer at least reserve items in size.
     *
     * Default = 1.
     */
    size_t reserve_items;

    /*!
     * Constrain the input buffer allocation size:
     * The scheduler may accumulate multiple buffers
     * into a single larger buffer under failure conditions.
     * The maximum size of this accumulated buffer
     * is constrained by this maximum_items setting.
     *
     * Default = 0 aka disabled.
     */
    size_t maximum_items;

    /*!
     * Set buffer inlining for this port config.
     * Inlining means that the input buffer can be used as an output buffer.
     * The goal is to make better use of cache and memory bandwidth.
     *
     * By default, inlining is disabled on all input ports.
     * The user should enable inlining on an input port
     * when it is understood that the work function will read
     * before writting to a particular section of the buffer.
     *
     * The scheduler will inline a buffer when
     *  * inlining is enabled on the particular input port
     *  * block holds the only buffer reference aka unique
     *  * the input buffer has the same affinity as the block
     *  * the input port has a buffer look-ahead of 0
     *
     * Default = false.
     */
    bool inline_buffer;

    /*!
     * Preload the input queue with num preload items.
     * All items preloaded into the buffer will be 0.
     * This is used to implement zero-padding for
     * things like sliding dot products/FIR filters.
     *
     *  - Increasing the preload at runtime will
     * inject more items into the input queue.
     *  - Decreasing the preload at runtime will
     * consume random items from the input queue.
     *
     * Default = 0.
     */
    size_t preload_items;
};

//! Configuration parameters for an output port
struct GRAS_API OutputPortConfig
{
    OutputPortConfig(void);

    //! The size of an item in bytes
    size_t item_size;

    /*!
     * Set an output reserve requirement such that work is called
     * with an output buffer at least reserve items in size.
     *
     * Default = 1.
     */
    size_t reserve_items;

    /*!
     * Constrain the output buffer allocation size:
     * The user might set a small maximum items
     * to reduce the amount of buffered items
     * waiting for processing in downstream queues.
     *
     * Default = 0 aka disabled.
     */
    size_t maximum_items;
};

} //namespace gras

#endif /*INCLUDED_GRAS_BLOCK_CONFIG_HPP*/
