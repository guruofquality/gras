// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_BLOCK_HPP
#define INCLUDED_GRAS_BLOCK_HPP

#include <gras/element.hpp>
#include <gras/sbuffer.hpp>
#include <gras/tag_iter.hpp>
#include <gras/tags.hpp>
#include <gras/work_buffer.hpp>
#include <gras/buffer_queue.hpp>
#include <vector>
#include <string>

namespace gras
{

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

    /*!
     * Force this block done when input port is done.
     * When the upstream feeding this port declares done,
     * this block will mark done once upstream notifies.
     * The primary usage is to modify the done logic
     * for the purposes of unit test confiruability.
     *
     * If the force done option is false, the block will
     * not mark done when this port's upstream is done.
     * However, this block will mark done when all
     * input ports are done, reguardless of this setting.
     *
     * Default = true.
     */
    bool force_done;
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

struct GRAS_API Block : Element
{
    //! Contruct an empty/null block
    Block(void);

    //! Create a new block given the name
    Block(const std::string &name);

    virtual ~Block(void);

    /*******************************************************************
     * Deal with input and output port configuration
     ******************************************************************/

    //! Get the configuration rules of an input port
    const InputPortConfig &input_config(const size_t which_input) const;

    //! Get the configuration rules of an input port
    InputPortConfig &input_config(const size_t which_input);

    //! Get the configuration rules of an output port
    const OutputPortConfig &output_config(const size_t which_output) const;

    //! Get the configuration rules of an output port
    OutputPortConfig &output_config(const size_t which_output);

    /*!
     * Commit changes to the port configuration.
     * Changes are commited automatically when the block becomes active.
     * However, once active, changes may not effect until commit_config().
     */
    void commit_config(void);

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

    /*!
     * Send a tag to the given input port on this block.
     * This is a thread-safe way for external scheduler
     * entities to post tags into the input of a block.
     * \param which_input an input port on this block
     * \param tag the tag to post to the input port
     */
    void post_input_tag(const size_t which_input, const Tag &tag);

    /*******************************************************************
     * Deal with message passing
     ******************************************************************/

    /*!
     * Send a message to the downstream on the given output port.
     * Messages are naturally asynchronous to stream and tag data.
     *
     * \param which_output the index of the output port
     * \param msg the message object to pass downstream
     */
    void post_output_msg(const size_t which_output, const PMCC &msg);

    /*!
     * Pop input message convenience routine.
     * This routine reads the first input message,
     * and erases this message from the given port.
     * The intention is to simplify the use case
     * for using this for port messages only.
     * If no message, the return value is null.
     *
     * \param which_input the index of the input port
     * \return the message on the front of the queue
     */
    PMCC pop_input_msg(const size_t which_input);

    /*!
     * Send a message to the given input port on this block.
     * This is a thread-safe way for external scheduler
     * entities to post messages into the input of a block.
     * \param which_input an input port on this block
     * \param msg the message to post to the input port
     */
    void post_input_msg(const size_t which_input, const PMCC &tag);

    /*******************************************************************
     * The property interface:
     * Provides polymorphic, thread-safe access to block properties.
     ******************************************************************/

    /*!
     * Register property getter method into the property interface.
     * Call register_getter() from the contructor of the block.
     *
     * Example register usage:
     * this->register_getter("foo", &MyBlock::get_foo);
     *
     * Example method declaration:
     * int get_foo(void);
     *
     * \param key the string to identify this property
     * \param get the class method to get the property
     */
    template <typename ClassType, typename ValueType>
    void register_getter(
        const std::string &key,
        ValueType(ClassType::*get)(void)
    );

    /*!
     * Register property setter method into the property interface.
     * Call register_setter() from the contructor of the block.
     *
     * Example register usage:
     * this->register_setter("foo", &MyBlock::set_foo);
     *
     * Example method declaration:
     * void set_foo(const int &new_foo);
     *
     * \param key the string to identify this property
     * \param set the class method to set the property
     */
    template <typename ClassType, typename ValueType>
    void register_setter(
        const std::string &key,
        void(ClassType::*set)(const ValueType &)
    );

    /*!
     * Set the value of a registered property.
     *
     * This call is synchronous and will not return until
     * the block has actually called the registered set operation.
     *
     * Note: the user must be careful to only use a value
     * that is of the exact type associated with this property.
     * Otherwise, set_property with throw a type error.
     *
     * Examples with explicit argument types:
     * my_block->set<size_t>("foo", 42);
     * my_block->set("foo", size_t(42));
     *
     * \param key the string to identify this property
     * \param value the new value to set to this property
     */
    template <typename ValueType>
    void set(const std::string &key, const ValueType &value);

    /*!
     * Get the value of a registered property with reference semantics.
     *
     * Note: the user must be careful to only use a value
     * that is of the exact type associated with this property.
     * Otherwise, get_property with throw a type error.
     *
     * Example getting property with reference semantics:
     * size_t foo; my_block->get("foo", foo);
     *
     * \param key the string to identify this property
     * \param value a reference to set to the result
     */
    template <typename ValueType>
    void get(const std::string &key, ValueType &value);

    /*!
     * Get the value of a registered property with return semantics.
     *
     * Example getting property with return value semantics:
     * const size_t foo = my_block->get<size_t>("foo");
     *
     * \param key the string to identify this property
     * \return the value of this property
     */
    template <typename ValueType>
    ValueType get(const std::string &key);

    /*******************************************************************
     * Work related routines and fail states
     ******************************************************************/

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

    /*******************************************************************
     * Direct buffer access API
     ******************************************************************/

    /*!
     * Get access to the underlying reference counted input buffer.
     * This is the same buffer pointed to by input_items[which].
     * This function must be called during the call to work().
     * Use this function to implement passive work-flows.
     *
     * \param which_input the input port index
     * \return a reference counted copy of the buffer
     */
    SBuffer get_input_buffer(const size_t which_input) const;

    /*!
     * Get access to the underlying reference counted output buffer.
     * This is the same buffer pointed to by output_items[which].
     * This function must be called during the call to work().
     * Use this to get a pool of buffers for datagram message ports.
     *
     * \param which_output the output port index
     * \return a reference counted copy of the buffer
     */
    SBuffer get_output_buffer(const size_t which_output) const;

    /*!
     * Remove a given number of bytes from the output buffer queue.
     * This call is intended to be used with get_output_buffer().
     * If pop_output_buffer() is not called after get_output_buffer(),
     * The full-size of the buffer will be automatically popped.
     *
     * \param which_output the output port index
     * \param num_bytes bytes to pop from the output buffer queue
     */
    void pop_output_buffer(const size_t which_output, const size_t num_bytes);

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
     * Post a buffer to the given input port on this block.
     * This is a thread-safe way for external scheduler
     * entities to post buffers into the input of a block.
     *
     * \param which_input an input port on this block
     * \param buffer the buffer to post to the input port
     */
    void post_input_buffer(const size_t which_input, const SBuffer &buffer);

    /*******************************************************************
     * Scheduler notification API
     ******************************************************************/

    /*!
     * Overload notify_active to get called when block becomes active.
     * This will be called when the TopBlock start/run API call executes.
     * The default implementation of notify_active is a NOP.
     */
    virtual void notify_active(void);

    /*!
     * Overload notify_inactive to get called when block becomes inactive.
     * This will be called when the TopBlock stop/run API call executes.
     * The default implementation of notify_inactive is a NOP.
     */
    virtual void notify_inactive(void);

    /*!
     * Overload notify_topology to get called on topological changes.
     * Use notify_topology to perform one-time resizing operations
     * to avoid a conditional resizing operation inside the work().
     */
    virtual void notify_topology(const size_t num_inputs, const size_t num_outputs);

    /*******************************************************************
     * routines related to affinity and allocation
     ******************************************************************/

    /*!
     * Set if the work call should be interruptible by stop().
     * Some work implementations block with the expectation of
     * getting a boost thread interrupt in a blocking call.
     * Set set_interruptible_work(true) if this is the case.
     * By default, work implementations are not interruptible.
     */
    void set_interruptible_work(const bool enb);

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
     * \param which_output the output port index number
     * \param config holds token and recommended length
     * \return a shared ptr to a new buffer queue object
     */
    virtual BufferQueueSptr output_buffer_allocator(
        const size_t which_output,
        const SBufferConfig &config
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
     * \param config holds token and recommended length
     * \return a shared ptr to a new buffer queue object
     */
    virtual BufferQueueSptr input_buffer_allocator(
        const size_t which_input,
        const SBufferConfig &config
    );

    /*******************************************************************
     * private implementation guts for overloads and template support
     ******************************************************************/
    virtual PMCC _handle_prop_access(const std::string &, const PMCC &, const bool);
    void _register_getter(const std::string &, PMCC);
    void _register_setter(const std::string &, PMCC);
    virtual void _set_property(const std::string &, const PMCC &);
    virtual PMCC _get_property(const std::string &);
};

} //namespace gras

#include <gras/detail/block.hpp>

#endif /*INCLUDED_GRAS_BLOCK_HPP*/
