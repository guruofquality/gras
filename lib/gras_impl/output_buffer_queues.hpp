// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_LIBGRAS_IMPL_OUTPUT_BUFFER_QUEUES_HPP
#define INCLUDED_LIBGRAS_IMPL_OUTPUT_BUFFER_QUEUES_HPP

#include <gras/buffer_queue.hpp>
#include <gras_impl/bitset.hpp>
#include <vector>

namespace gras
{

struct OutputBufferQueues
{

    void set_buffer_queue(const size_t i, BufferQueueSptr queue)
    {
        _queues[i] = queue;
        _update(i);
    }

    void set_reserve_bytes(const size_t i, const size_t num_bytes)
    {
        _reserve_bytes[i] = num_bytes;
        _update(i);
    }

    GRAS_FORCE_INLINE void resize(const size_t size)
    {
        _bitset.resize(size);
        _queues.resize(size);
        _reserve_bytes.resize(size, 1);
    }

    GRAS_FORCE_INLINE void push(const size_t i, const SBuffer &value)
    {
        _queues[i]->push(value);
        _update(i);
    }

    GRAS_FORCE_INLINE void flush_all(void)
    {
        const size_t old_size = this->size();
        this->resize(0);
        this->resize(old_size);
    }

    //! used for input buffer inlining
    /*
    GRAS_FORCE_INLINE void push_front(const size_t i, const T &value)
    {
        ASSERT(not _queues[i].full());
        _queues[i].push_front(value);
        _bitset.set(i);
    }
    */

    GRAS_FORCE_INLINE SBuffer &front(const size_t i)
    {
        ASSERT(not _queues[i]->empty());
        return _queues[i]->front();
    }

    GRAS_FORCE_INLINE void pop(const size_t i)
    {
        _queues[i]->pop();
        _update(i);
    }

    GRAS_FORCE_INLINE void fail(const size_t i)
    {
        _bitset.reset(i);
    }
/*
    GRAS_FORCE_INLINE void flush(const size_t i)
    {
        _queues[i].clear();
        _bitset.reset(i);
    }

    GRAS_FORCE_INLINE void flush_all(void)
    {
        for (size_t i = 0; i < this->size(); i++) this->flush(i);
    }
*/
    GRAS_FORCE_INLINE bool ready(const size_t i) const
    {
        if (_queues[i]->empty()) return false;
        const SBuffer &front = _queues[i]->front();
        const size_t avail = front.get_actual_length() - front.offset -  front.length;
        return avail >= _reserve_bytes[i];
    }

    GRAS_FORCE_INLINE bool empty(const size_t i) const
    {
        return _queues[i]->empty();
    }

    GRAS_FORCE_INLINE bool all_ready(void) const
    {
        return _bitset.all();
    }

    GRAS_FORCE_INLINE size_t size(void) const
    {
        return _queues.size();
    }

    GRAS_FORCE_INLINE void _update(const size_t i)
    {
        _bitset.set(i, this->ready(i));
    }

    BitSet _bitset;
    std::vector<BufferQueueSptr> _queues;
    std::vector<size_t> _reserve_bytes;

};

} //namespace gras

#endif /*INCLUDED_LIBGRAS_IMPL_OUTPUT_BUFFER_QUEUES_HPP*/
