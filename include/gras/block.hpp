// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_BLOCK_HPP
#define INCLUDED_GRAS_BLOCK_HPP

#include <gras/element.hpp>
#include <gras/sbuffer.hpp>
#include <gras/tags.hpp>
#include <vector>
#include <string>
#include <boost/range.hpp> //iterator range

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

template <typename PtrType> struct WorkBuffer
{
    //! get a native pointer type to this buffer
    inline PtrType get(void) const
    {
        return _mem;
    }

    //! get a pointer of the desired type to this buffer
    template <typename T> inline T cast(void) const
    {
        return reinterpret_cast<T>(this->get());
    }

    //! get the number of items in this buffer
    inline size_t size(void) const
    {
        return _len;
    }

    //! Get the memory pointer reference
    inline PtrType &get(void)
    {
        return _mem;
    }

    //! Get the buffer length reference
    inline size_t &size(void)
    {
        return _len;
    }

    PtrType _mem;
    size_t _len;
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
    InputPortConfig input_config(const size_t which_input = 0) const;

    //! Set the configuration rules for an input port
    void set_input_config(const InputPortConfig &config, const size_t which_input = 0);

    //! Get the configuration rules of an output port
    OutputPortConfig output_config(const size_t which_output = 0) const;

    //! Set the configuration rules for an output port
    void set_output_config(const OutputPortConfig &config, const size_t which_output = 0);

    /*******************************************************************
     * Deal with data production and consumption
     ******************************************************************/

    //! Call during work to consume items
    void consume(const size_t which_input, const size_t how_many_items);

    //! Call during work to produce items
    void produce(const size_t which_output, const size_t how_many_items);

    /*******************************************************************
     * Deal with tag handling and tag configuration
     ******************************************************************/

    item_index_t nitems_read(const size_t which_input);

    item_index_t nitems_written(const size_t which_output);

    //! Send a tag to the downstream on the given output port
    void post_output_tag(const size_t which_output, const Tag &tag);

    //! Iterator return type get_input_tags - stl and boost compliant
    typedef boost::iterator_range<std::vector<Tag>::const_iterator> TagIter;

    //! Get an iterator of item tags for the given input
    TagIter get_input_tags(const size_t which_input = 0);

    //! Overload me to implement tag propagation logic
    virtual void propagate_tags(const size_t which_input, const TagIter &iter);

    /*******************************************************************
     * Work related routines from basic block
     ******************************************************************/

    //! Called when the flow graph is started, can overload
    virtual bool start(void);

    //! Called when the flow graph is stopped, can overload
    virtual bool stop(void);

    typedef std::vector<WorkBuffer<const void *> > InputItems;
    typedef std::vector<WorkBuffer<void *> > OutputItems;

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
