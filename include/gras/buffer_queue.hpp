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

    //! Create a buffer queue using the pool allocator
    GRAS_API static BufferQueueSptr make_pool(const SBufferConfig &config, const size_t num_buffs);

    //! Create a buffer queue using the circular buffer allocator
    GRAS_API static BufferQueueSptr make_circ(const SBufferConfig &config);

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
