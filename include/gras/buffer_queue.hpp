// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_BUFFER_QUEUE_HPP
#define INCLUDED_GRAS_BUFFER_QUEUE_HPP

#include <gras/gras.hpp>
#include <gras/sbuffer.hpp>
#include <boost/shared_ptr.hpp>

namespace gras
{

struct BufferQueue;

typedef boost::shared_ptr<BufferQueue> BufferQueueSptr;

//! Buffer Queue is an interface enabling us to create custom buffer allocators.
struct BufferQueue
{
    //! virtual destructor
    virtual ~BufferQueue(void){}

    /*!
     * Create a buffer queue object using the pool allocator.
     * A pool of buffers contains individual buffer allocations.
     *
     * The config param is used to pass information
     * about per-buffer size, affinity, and token.
     *
     * \param config used to alloc one buffer
     * \param num_buffs alloc this many buffs
     * \return a new buffer queue sptr
     */
    GRAS_API static BufferQueueSptr make_pool(
        const SBufferConfig &config,
        const size_t num_buffs
    );

    /*!
     * Create a buffer queue object using the circular allocator.
     * The circular allocator contains one large double-mapped buffer.
     * This buffer is virtually mapped so that buff[n+len] = buff[n].
     *
     * Pieces of this buffer can be passed around in smaller chunks.
     * The number of chunks is dictated by num_buffs,
     * the size of the chunks is dictated by config.length.
     * The size of the circular buffer will be num_buffs*config.length
     *
     * \param config used to alloc one buffer
     * \param num_buffs this many smaller chunks
     *
     * \return a new buffer queue sptr
     */
    GRAS_API static BufferQueueSptr make_circ(
        const SBufferConfig &config,
        const size_t num_buffs
    );

    //! Get a reference to the buffer at the front of the queue
    virtual SBuffer &front(void) = 0;

    //! Pop off the used portion of the queue
    virtual void pop(void) = 0;

    //! Push a used buffer back into the queue
    virtual void push(const SBuffer &buff) = 0;

    //! Is the queue empty?
    virtual bool empty(void) const = 0;

};

} //namespace gras

#endif /*INCLUDED_GRAS_BUFFER_QUEUE_HPP*/
