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
    std::string name; //for debug

    void set_buffer_queue(const size_t i, BufferQueueSptr queue)
    {
        _queues[i] = queue;
        _update(i);
    }

    void set_reserve_bytes(const size_t i, const size_t num_bytes)
    {
        _reserve_bytes[i] = num_bytes;
        if (_queues[i]) _update(i);
    }

    GRAS_FORCE_INLINE void resize(const size_t size)
    {
        _bitset.resize(size);
        _queues.resize(size);
        _reserve_bytes.resize(size, 1);
        _inline_buffer.resize(size);
    }

    GRAS_FORCE_INLINE void push(const size_t i, const SBuffer &buff)
    {
        if (not _queues[i]) return; //block is likely done, throw out buffer
        _queues[i]->push(buff);
        _update(i);
    }

    GRAS_FORCE_INLINE void flush_all(void)
    {
        const size_t old_size = this->size();
        this->resize(0);
        this->resize(old_size);
    }

    GRAS_FORCE_INLINE SBuffer &front(const size_t i)
    {
        if GRAS_UNLIKELY(_inline_buffer[i]) return _inline_buffer[i];
        ASSERT(not this->empty(i));
        return _queues[i]->front();
    }

    GRAS_FORCE_INLINE void consume(const size_t i)
    {
        if GRAS_UNLIKELY(_inline_buffer[i])
        {
            _inline_buffer[i].reset();
            return;
        }

        ASSERT(not this->empty(i));
        SBuffer &buff = this->front(i);
        if GRAS_UNLIKELY(buff.length == 0) return;

        //increment buffer for next use
        buff.offset += buff.length;
        buff.length = 0;

        this->pop(i);
    }

    GRAS_FORCE_INLINE void pop(const size_t i)
    {
        if GRAS_UNLIKELY(_inline_buffer[i])
        {
            _inline_buffer[i].reset();
            return;
        }

        ASSERT(_queues[i]);
        ASSERT(not _queues[i]->empty());
        _queues[i]->pop();
        _update(i);
    }

    GRAS_FORCE_INLINE void fail(const size_t i)
    {
        _bitset.reset(i);
    }

    GRAS_FORCE_INLINE bool ready(const size_t i) const
    {
        return _bitset[i];
    }

    GRAS_FORCE_INLINE bool empty(const size_t i) const
    {
        if GRAS_UNLIKELY(_inline_buffer[i]) return false;
        return (not _queues[i] or _queues[i]->empty());
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
        if (not _queues[i] or _queues[i]->empty())
        {
            _bitset.reset(i);
            return;
        }
        const SBuffer &front = _queues[i]->front();
        const size_t avail = front.get_actual_length() - front.offset -  front.length;
        _bitset.set(i, avail >= _reserve_bytes[i]);
    }

    GRAS_FORCE_INLINE void set_inline(const size_t i, const SBuffer &inline_buffer)
    {
        _inline_buffer[i] = inline_buffer;
        _inline_buffer[i].length = 0;
        _bitset.set(i);
    }

    BitSet _bitset;
    std::vector<BufferQueueSptr> _queues;
    std::vector<size_t> _reserve_bytes;
    std::vector<SBuffer> _inline_buffer;

};

} //namespace gras

#endif /*INCLUDED_LIBGRAS_IMPL_OUTPUT_BUFFER_QUEUES_HPP*/
