//
// Copyright 2012 Josh Blum
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef INCLUDED_LIBGRAS_IMPL_INPUT_BUFFERS_HPP
#define INCLUDED_LIBGRAS_IMPL_INPUT_BUFFERS_HPP

#include <gras_impl/debug.hpp>
#include <gras_impl/bitset.hpp>
#include <gras_impl/buffer_queue.hpp>
#include <gnuradio/sbuffer.hpp>
#include <vector>
#include <queue>
#include <deque>
#include <cstring> //memcpy/memset
#include <boost/circular_buffer.hpp>

namespace gnuradio
{

struct InputBufferQueues
{
    enum {MAX_QUEUE_SIZE = 128};
    enum {MAX_AUX_BUFF_BYTES=(1<<16)};

    ~InputBufferQueues(void)
    {
        this->resize(0);
    }

    void update_history_bytes(const size_t i, const size_t hist_bytes);

    //! Call to get an input buffer for work
    SBuffer front(const size_t i, bool &potential_GRAS_FORCE_INLINE);

    //! Call when input bytes consumed by work
    void consume(const size_t i, const size_t bytes_consumed);

    void resize(const size_t size);

    GRAS_FORCE_INLINE void push(const size_t i, const SBuffer &buffer)
    {
        ASSERT(not _queues[i].full());
        _queues[i].push_back(buffer);
        _enqueued_bytes[i] += _queues[i].back().length;
        __update(i);
    }

    GRAS_FORCE_INLINE void flush(const size_t i)
    {
        _queues[i].clear();
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
        return not _bitset[i];
    }

    GRAS_FORCE_INLINE bool all_ready(void) const
    {
        return _bitset.all();
    }

    void __prepare(const size_t i);

    GRAS_FORCE_INLINE void __update(const size_t i)
    {
        _bitset.set(i, _enqueued_bytes[i] != 0);
    }

    BitSet _bitset;
    std::vector<size_t> _enqueued_bytes;
    std::vector<boost::circular_buffer<SBuffer> > _queues;
    std::vector<size_t> _history_bytes;
    std::vector<boost::shared_ptr<BufferQueue> > _aux_queues;
};


GRAS_FORCE_INLINE void InputBufferQueues::resize(const size_t size)
{
    _bitset.resize(size);
    _enqueued_bytes.resize(size, 0);
    _queues.resize(size, boost::circular_buffer<SBuffer>(MAX_QUEUE_SIZE));
    _history_bytes.resize(size, 0);
    _aux_queues.resize(size);

    for (size_t i = 0; i < this->size(); i++)
    {
        if (_aux_queues[i]) continue;
        _aux_queues[i] = boost::shared_ptr<BufferQueue>(new BufferQueue());
        _aux_queues[i]->allocate_one(MAX_AUX_BUFF_BYTES);
        _aux_queues[i]->allocate_one(MAX_AUX_BUFF_BYTES);
    }

}

inline void InputBufferQueues::update_history_bytes(const size_t i, const size_t hist_bytes)
{
    //there is history, so enqueue some initial history
    if (hist_bytes > _history_bytes[i])
    {
        SBuffer buff = _aux_queues[i]->front();
        _aux_queues[i]->pop();

        const size_t delta = hist_bytes - _history_bytes[i];
        std::memset(buff.get_actual_memory(), 0, delta);
        buff.offset = 0;
        buff.length = delta;

        this->push(i, buff);
    }
    if (hist_bytes < _history_bytes[i])
    {
        size_t delta = _history_bytes[i] - hist_bytes;
        delta = std::min(delta, _enqueued_bytes[i]); //FIXME
        //TODO consume extra delta on push...? so we dont need std::min
        this->consume(i, delta);
    }

    _history_bytes[i] = hist_bytes;
}

GRAS_FORCE_INLINE SBuffer InputBufferQueues::front(const size_t i, bool &potential_inline)
{
    //if (_queues[i].empty()) return BuffInfo();

    ASSERT(not _queues[i].empty());
    ASSERT(this->ready(i));
    __prepare(i);
    SBuffer &front = _queues[i].front();
    const bool unique = front.unique();

    //same buffer, different offset and length
    SBuffer buff = front;

    //set the flag that this buffer *might* be inlined as an output buffer
    potential_inline = unique and (buff.length == front.length);

    return buff;
}

GRAS_FORCE_INLINE void InputBufferQueues::__prepare(const size_t i)
{
    SBufferConfig config;
    config.memory = NULL;
    config.length = _enqueued_bytes[i];
    SBuffer newbuff(config);
    newbuff.offset = 0;
    newbuff.length = 0;

    while (not _queues[i].empty())
    {
        std::memcpy(newbuff.get(newbuff.length), _queues[i].front().get(), _queues[i].front().length);
        newbuff.length += _queues[i].front().length;
        _queues[i].pop_front();
    }

    _queues[i].push_back(newbuff);
    return;
    
    
    
    
    /*
    
    
    
    
    //HERE();
    //assumes that we are always pushing proper history buffs on front
    //ASSERT(_queues[i].front().length >= _history_bytes[i]);

    while (_queues[i].front().length < _reserve_bytes[i])
    {
        SBuffer &front = _queues[i].front();
        SBuffer dst;

        //do we need a new buffer:
        //- is the buffer unique (queue has only reference)?
        //- can its remaining space meet reserve requirements?
        const bool enough_space = front.get_actual_length() >= _reserve_bytes[i] + front.offset;
        if (enough_space and front.unique())
        {
            dst = _queues[i].front();
            _queues[i].pop_front();
        }
        else
        {
            dst = _aux_queues[i]->front();
            _aux_queues[i]->pop();
            dst.offset = 0;
            dst.length = 0;
            _in_aux_buff[i] = true;
        }

        SBuffer src = _queues[i].front();
        _queues[i].pop_front();
        const size_t dst_tail = dst.get_actual_length() - (dst.offset + dst.length);
        const size_t bytes = std::min(dst_tail, src.length);
        //const size_t bytes = std::min(std::min(dst_tail, src.length), _post_bytes[i]);
        std::memcpy(dst.get(dst.length), src.get(), bytes);

        //update buffer additions, consumptions
        dst.length += bytes;
        src.offset += bytes;
        src.length -= bytes;

        //keep the source buffer if not fully consumed
        if (src.length) _queues[i].push_front(src);

        //destination buffer is the new front of the queue
        _queues[i].push_front(dst);
    }
    */
}


GRAS_FORCE_INLINE void InputBufferQueues::consume(const size_t i, const size_t bytes_consumed)
{
    //if (bytes_consumed == 0) return true;

    //assert that we dont consume past the bounds of the buffer
    ASSERT(_queues[i].front().length >= bytes_consumed);

    //update bounds on the current buffer
    _queues[i].front().offset += bytes_consumed;
    _queues[i].front().length -= bytes_consumed;

    ASSERT(_queues[i].front().offset <= _queues[i].front().get_actual_length());

    //safe to pop here when the buffer is consumed and no history
    //if (_queues[i].front().length == 0/* and _history_bytes[i] == 0*/)
    {
    //    _queues[i].pop_front();
    }
/*
    else if (_in_aux_buff[i] and _queues[i].front().offset >= 2*_history_bytes[i])
    {
        const SBuffer buff = _queues[i].front();
        _queues[i].pop_front();

        if (_queues[i].empty())
        {
            _queues[i].push_front(buff);
        }
        else
        {
            _in_aux_buff[i] = false;
            const size_t residual = buff.length;
            _queues[i].front().length += residual;
            _queues[i].front().offset -= residual;
        }
    }
    */

    //update the number of bytes in this queue
    ASSERT(_enqueued_bytes[i] >= bytes_consumed);
    _enqueued_bytes[i] -= bytes_consumed;

    __update(i);

}

} //namespace gnuradio

#endif /*INCLUDED_LIBGRAS_IMPL_INPUT_BUFFERS_HPP*/
