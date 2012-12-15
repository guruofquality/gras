// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_LIBGRAS_IMPL_INPUT_BUFFERS_HPP
#define INCLUDED_LIBGRAS_IMPL_INPUT_BUFFERS_HPP

#include <gras_impl/debug.hpp>
#include <gras_impl/bitset.hpp>
#include <gras_impl/simple_buffer_queue.hpp>
#include <gras/sbuffer.hpp>
#include <vector>
#include <queue>
#include <deque>
#include <cstring> //memcpy/memset
#include <boost/circular_buffer.hpp>

namespace gras
{

struct InputBufferQueues
{
    enum {MAX_QUEUE_SIZE = 128};
    enum {MAX_AUX_BUFF_BYTES=(1<<16)};

    static SBuffer make_null_buff(void)
    {
        SBufferConfig config;
        config.memory = NULL;
        config.length = 1;
        return SBuffer(config);
    }

    static SBuffer &get_null_buff(void)
    {
        static SBuffer null = make_null_buff();
        null.offset = 0;
        null.length = 0;
        return null;
    }

    ~InputBufferQueues(void)
    {
        this->resize(0);
    }

    void update_config(const size_t i, const size_t, const size_t, const size_t, const size_t);

    //! Call to get an input buffer for work
    GRAS_FORCE_INLINE const SBuffer &front(const size_t i)
    {
        ASSERT(this->ready(i));

        //special case when the null buffer is possible
        if (_queues[i].empty()) return get_null_buff();

        //there are enough enqueued bytes, but not in the front buffer
        const bool must_accumulate = _queues[i].front().length < _reserve_bytes[i];

        //should we accumulate? a guess at heuristic improvement
        const bool heavy_load = _enqueued_bytes[i] >= _maximum_bytes[i];
        const bool light_front = _queues[i].front().length <= _maximum_bytes[i]/2;
        const bool should_accumulate = heavy_load and light_front;

        if (must_accumulate or should_accumulate) this->accumulate(i);

        ASSERT(_queues[i].front().length >= _reserve_bytes[i]);

        ASSERT((_queues[i].front().length % _items_sizes[i]) == 0);

        return _queues[i].front();
    }

    //! Call when input bytes consumed by work
    void consume(const size_t i, const size_t bytes_consumed);

    void resize(const size_t size);

    void accumulate(const size_t i);

    /*!
     * Can we consider this queue's buffers to be accumulated?
     * Either the first buffer holds all of the enqueued bytes
     * or the first buffer is larger than we can accumulate.
     */
    GRAS_FORCE_INLINE bool is_accumulated(const size_t i) const
    {
        return (_queues[i].size() <= 1) or this->is_front_maximal(i);
    }

    //! Return true if the front buffer is at least max size
    GRAS_FORCE_INLINE bool is_front_maximal(const size_t i) const
    {
        ASSERT(not _queues[i].empty());
        return _queues[i].front().length >= _maximum_bytes[i];
    }

    GRAS_FORCE_INLINE void pop(const size_t i)
    {
        ASSERT(not _queues[i].empty());
        _queues[i].front().reset();
        _queues[i].pop_front();
    }

    GRAS_FORCE_INLINE void push(const size_t i, const SBuffer &buffer)
    {
        ASSERT(not _queues[i].full());
        if (buffer.length == 0) return;

        //does this buffer starts where the last one ends?
        //perform buffer stitching into back of buffer
        if (
            not _queues[i].empty() and _queues[i].back() == buffer and
            (_queues[i].back().length + _queues[i].back().offset) == buffer.offset
        ){
            _queues[i].back().length += buffer.length;
        }
        else
        {
            _queues[i].push_back(buffer);
        }

        _enqueued_bytes[i] += buffer.length;
        __update(i);
    }

    GRAS_FORCE_INLINE void flush(const size_t i)
    {
        _queues[i].clear();
        _bitset.reset(i);
    }

    GRAS_FORCE_INLINE void fail(const size_t i)
    {
        _bitset.reset(i);
    }

    size_t size(void) const
    {
        return _queues.size();
    }

    GRAS_FORCE_INLINE void flush_all(void)
    {
        const size_t old_size = this->size();
        this->resize(0);
        this->resize(old_size);
    }

    GRAS_FORCE_INLINE bool ready(const size_t i) const
    {
        return _bitset[i];
    }

    GRAS_FORCE_INLINE bool empty(const size_t i) const
    {
        return _queues[i].empty();
    }

    GRAS_FORCE_INLINE bool all_ready(void) const
    {
        return _bitset.all();
    }

    GRAS_FORCE_INLINE void __update(const size_t i)
    {
        _bitset.set(i, _enqueued_bytes[i] >= _reserve_bytes[i]);
    }

    BitSet _bitset;
    std::vector<size_t> _items_sizes;
    std::vector<size_t> _enqueued_bytes;
    std::vector<size_t> _reserve_bytes;
    std::vector<size_t> _maximum_bytes;
    std::vector<boost::circular_buffer<SBuffer> > _queues;
    std::vector<size_t> _preload_bytes;
    std::vector<boost::shared_ptr<SimpleBufferQueue> > _aux_queues;
};


GRAS_FORCE_INLINE void InputBufferQueues::resize(const size_t size)
{
    _bitset.resize(size);
    _items_sizes.resize(size, 0);
    _enqueued_bytes.resize(size, 0);
    _reserve_bytes.resize(size, 1);
    _maximum_bytes.resize(size, MAX_AUX_BUFF_BYTES);
    _queues.resize(size, boost::circular_buffer<SBuffer>(MAX_QUEUE_SIZE));
    _preload_bytes.resize(size, 0);
    _aux_queues.resize(size);

}

inline void InputBufferQueues::update_config(
    const size_t i,
    const size_t item_size,
    const size_t preload_bytes,
    const size_t reserve_bytes,
    const size_t maximum_bytes
)
{
    _items_sizes[i] = item_size;

    //first allocate the aux buffer
    if (maximum_bytes != 0) _maximum_bytes[i] = maximum_bytes;
    _maximum_bytes[i] = std::max(_maximum_bytes[i], reserve_bytes);
    if (
        not _aux_queues[i] or
        _aux_queues[i]->empty() or
        _aux_queues[i]->front().get_actual_length() != _maximum_bytes[i]
    ){
        _aux_queues[i].reset(new SimpleBufferQueue());
        _aux_queues[i]->allocate_one(_maximum_bytes[i]);
        _aux_queues[i]->allocate_one(_maximum_bytes[i]);
        _aux_queues[i]->allocate_one(_maximum_bytes[i]);
    }

    //there is preload, so enqueue some initial preload
    if (preload_bytes > _preload_bytes[i])
    {
        SBuffer buff = _aux_queues[i]->front();
        _aux_queues[i]->pop();

        const size_t delta = preload_bytes - _preload_bytes[i];
        std::memset(buff.get_actual_memory(), 0, delta);
        buff.offset = 0;
        buff.length = delta;

        this->push(i, buff);
    }
    if (preload_bytes < _preload_bytes[i])
    {
        size_t delta = _preload_bytes[i] - preload_bytes;
        delta = std::min(delta, _enqueued_bytes[i]); //FIXME
        //TODO consume extra delta on push...? so we dont need std::min
        this->consume(i, delta);
    }

    _preload_bytes[i] = preload_bytes;
    _reserve_bytes[i] = reserve_bytes;
    this->__update(i);
}

GRAS_FORCE_INLINE void InputBufferQueues::accumulate(const size_t i)
{
    if (this->is_accumulated(i)) return;

    if (_aux_queues[i]->empty())
    {
        _aux_queues[i]->allocate_one(_maximum_bytes[i]);
    }
    ASSERT(not _aux_queues[i]->empty());

    SBuffer accum_buff = _aux_queues[i]->front();
    _aux_queues[i]->pop();
    accum_buff.offset = 0;
    accum_buff.length = 0;

    size_t free_bytes = accum_buff.get_actual_length();
    free_bytes /= _items_sizes[i]; free_bytes *= _items_sizes[i];

    while (not _queues[i].empty() and free_bytes != 0)
    {
        SBuffer &front = _queues[i].front();
        const size_t bytes = std::min(front.length, free_bytes);
        std::memcpy(accum_buff.get(accum_buff.length), front.get(), bytes);
        accum_buff.length += bytes;
        free_bytes -= bytes;
        front.length -= bytes;
        front.offset += bytes;
        if (front.length == 0) this->pop(i);
    }

    _queues[i].push_front(accum_buff);

    ASSERT(this->is_accumulated(i));
}

GRAS_FORCE_INLINE void InputBufferQueues::consume(const size_t i, const size_t bytes_consumed)
{
    if (bytes_consumed == 0) return;

    ASSERT(not _queues[i].empty());
    SBuffer &front = _queues[i].front();

    //assert that we dont consume past the bounds of the buffer
    ASSERT(front.length >= bytes_consumed);

    //update bounds on the current buffer
    front.offset += bytes_consumed;
    front.length -= bytes_consumed;
    ASSERT(front.offset <= front.get_actual_length());
    ASSERT((_queues[i].front().length % _items_sizes[i]) == 0);
    if (front.length == 0) this->pop(i);

    //update the number of bytes in this queue
    ASSERT(_enqueued_bytes[i] >= bytes_consumed);
    _enqueued_bytes[i] -= bytes_consumed;

    __update(i);
}

} //namespace gras

#endif /*INCLUDED_LIBGRAS_IMPL_INPUT_BUFFERS_HPP*/
