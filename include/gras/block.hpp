// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_BLOCK_HPP
#define INCLUDED_GRAS_BLOCK_HPP

#include <gras/element.hpp>
#include <gras/sbuffer.hpp>
#include <gras/tags.hpp>
#include <gras/work_buffer.hpp>
#include <vector>
#include <string>

namespace gras
{

//! Configuration parameters for an input port
struct GRAS_API InputPortConfig
{
    InputPortConfig(void);

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
     * Set the number of input buffer look-ahead items.
     * When num look-ahead items are not consumed,
     * they will be available for the next work call.
     * This is used to implement sample memory for
     * things like sliding dot products/FIR filters.
     *
     * Default = 0.
     */
    size_t lookahead_items;
};

//! Configuration parameters for an output port
struct GRAS_API OutputPortConfig
{
    OutputPortConfig(void);

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

struct GRAS_API Block : Element
{

    //! Contruct an empty/null block
    Block(void);

    //! Create a new block given the name
    Block(const std::string &name);

    /*******************************************************************
     * Deal with input and output port configuration
     ******************************************************************/

    //! Get the configuration rules of an input port
    InputPortConfig get_input_config(const size_t which_input) const;

    //! Set the configuration rules for an input port
    void set_input_config(const size_t which_input, const InputPortConfig &config);

    //! Get the configuration rules of an output port
    OutputPortConfig get_output_config(const size_t which_output) const;

    //! Set the configuration rules for an output port
    void set_output_config(const size_t which_output, const OutputPortConfig &config);

    /*******************************************************************
     * Deal with data production and consumption
     ******************************************************************/

    //! Call during work to consume items
    void consume(const size_t which_input, const size_t num_items);

    //! Call during work to produce items
    void produce(const size_t which_output, const size_t num_items);

    //! Convenience method to consume items on all inputs
    void consume(const size_t num_items);

    //! Convenience method to produce items on all outputs
    void produce(const size_t num_items);

    //! Get absolute count of all items consumed on the given input port
    item_index_t get_consumed(const size_t which_input);

    //! Get absolute count of all items produced on the given output port
    item_index_t get_produced(const size_t which_output);

    /*******************************************************************
     * Deal with tag handling and tag configuration
     ******************************************************************/

    //! Send a tag to the downstream on the given output port
    void post_output_tag(const size_t which_output, const Tag &tag);

    //! Get an iterator of item tags for the given input
    TagIter get_input_tags(const size_t which_input);

    /*!
     * Erase all tags on the given input port.
     * This method may be called from the work() context
     * to erase all of the queued up tags on the input.
     * Once erased, messages cannot be propagated downstream.
     * This method allows a user to treat an input port
     * as an async message source without a data stream.
     * In this case, after processing messages from get_input_tags(),
     * the user should call erase_input_tags() before retuning from work().
     */
    void erase_input_tags(const size_t which_input);

    /*!
     * Overload me to implement custom tag propagation logic:
     *
     * Propagate tags will be given an iterator for all input tags
     * whose offset counts is less than the number of items consumed.
     * It is the job of the propagate_tags overloaded function to
     * propagate tags to the downstream and interpolate the offset.
     *
     * By default, the propagate_tags implementation is to:
     * broadcast each consumed input tags to all output ports
     * using the local input offset as the local output offset.
     *
     * Also, the user may simply propagate tags from within work.
     */
    virtual void propagate_tags(const size_t which_input, const TagIter &iter);

    /*******************************************************************
     * Work related routines and fail states
     ******************************************************************/

    //! Called when the flow graph is started, can overload
    virtual bool start(void);

    //! Called when the flow graph is stopped, can overload
    virtual bool stop(void);

    typedef WorkBufferArray<const void *> InputItems;
    typedef WorkBufferArray<void *> OutputItems;

    //! The official call into the work routine (overload please)
    virtual void work(
        const InputItems &input_items,
        const OutputItems &output_items
    ) = 0;

    /*!
     * Tell the scheduler that an output requirement could not be met.
     *
     * - If the output buffer was partially filled (ie, not flushed downstream),
     * this will cause the output buffer to flush to the downstream.
     * The next call to work will be with a full size output buffer.
     *
     * - If the output buffer was not partially filled, this call will throw.
     * In this case, the user should set larger maximum_items on this port.
     *
     * \param which_output the output port index
     */
    void mark_output_fail(const size_t which_output);

    /*!
     * Tell the scheduler that an input requirement could not be met.
     *
     *  - If there are more inputs enqueued ahead of this buffer,
     * the enqueued inputs will be accumulated into a larger buffer.
     * The next call to work will be with a larger input buffer.
     *
     * - If the buffer is already accumlated and the upstream provider
     * is no longer producing, then the scheduler will mark this block done.
     *
     * - If the input buffer at the maximum size, this call will throw.
     * In this case, the user should set larger maximum_items on this port.
     *
     * \param which_input the input port index
     */
    void mark_input_fail(const size_t which_input);

    /*!
     * Mark this block as done.
     * The scheduler will no longer call the work() routine.
     * Downstream consumers and upstream providers will be notified.
     */
    void mark_done(void);

    /*!
     * Get access to the underlying reference counted input buffer.
     * This is the same buffer pointed to by input_items[which].
     * This function must be called during the call to work().
     * Use this function to implement passive work-flows.
     *
     * \param which_input the input port index
     * \return a const reference to the buffer
     */
    const SBuffer &get_input_buffer(const size_t which_input) const;

    /*!
     * Get access to the underlying reference counted output buffer.
     * This is the same buffer pointed to by output_items[which].
     * This function must be called during the call to work().
     * Use this to get a pool of buffers for datagram message ports.
     *
     * \param which_output the output port index
     * \return a const reference to the buffer
     */
    const SBuffer &get_output_buffer(const size_t which_output) const;

    /*!
     * Post the given output buffer to the downstream.
     * This function must be called during the call to work().
     * Use this function to implement passive work-flows.
     *
     * Take the following rules into account:
     *  - The buffer will be immediately sent to the downstream.
     *  - The value for get_produced will automatically increase.
     *  - buffer.length should be in number of bytes (not items).
     *  - Do not call produce() for items in this buffer.
     *  - Call post_output_tag() before post_output_buffer().
     *
     * \param which_output the output port index
     * \param buffer the buffer to send downstream
     */
    void post_output_buffer(const size_t which_output, const SBuffer &buffer);

    /*!
     * Overload notify_topology to get called on topological changes.
     * Use notify_topology to perform one-time resizing operations
     * to avoid a conditional resizing operation inside the work().
     */
    virtual void notify_topology(const size_t num_inputs, const size_t num_outputs);

    /*!
     * Set if the work call should be interruptible by stop().
     * Some work implementations block with the expectation of
     * getting a boost thread interrupt in a blocking call.
     * Set set_interruptible_work(true) if this is the case.
     * By default, work implementations are not interruptible.
     */
    void set_interruptible_work(const bool enb);

    /*******************************************************************
     * routines related to affinity and allocation
     ******************************************************************/

    /*!
     * Set the node affinity of this block.
     * This call affects how output buffers are allocated.
     * By default memory is allocated by malloc.
     * When the affinity is set, virtual memory
     * will be locked to a physical CPU/memory node.
     * \param affinity a memory node on the system
     */
    void set_buffer_affinity(const long affinity);

    /*!
     * The output buffer allocator method.
     * This method is called by the scheduler to allocate output buffers.
     * The user may overload this method to create a custom allocator.
     *
     * Example use case:
     * //TODO code example
     *
     * \param which_output the output port index number
     * \param token the token for the buffer's returner
     * \param recommend_length the schedulers recommended length in bytes
     * \return the token used for the buffer allocation (may be the same)
     */
    virtual SBufferToken output_buffer_allocator(
        const size_t which_output,
        const SBufferToken &token,
        const size_t recommend_length
    );

    /*!
     * The input buffer allocator method.
     * This method is special and very different from allocate output buffers.
     * Typically, blocks do not have control of their input buffers.
     * When overloaded, an upstream block will ask this block
     * to allocate its output buffers. This way, this block will get
     * input buffers which were actually allocated by this method.
     *
     * \param which_input the input port index number
     * \param token the token for the buffer's returner
     * \param recommend_length the schedulers recommended length in bytes
     * \return the token used for the buffer allocation (may be the same)
     */
    virtual SBufferToken input_buffer_allocator(
        const size_t which_input,
        const SBufferToken &token,
        const size_t recommend_length
    );

};

} //namespace gras

#endif /*INCLUDED_GRAS_BLOCK_HPP*/
