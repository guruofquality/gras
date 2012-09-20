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
// along with io_sig program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef INCLUDED_LIBGRAS_IMPL_INPUT_BUFFERS_HPP
#define INCLUDED_LIBGRAS_IMPL_INPUT_BUFFERS_HPP

#include <gras_impl/debug.hpp>
#include <gras_impl/buffer_queue.hpp>
#include <gnuradio/sbuffer.hpp>
#include <boost/dynamic_bitset.hpp>
#include <vector>
#include <queue>
#include <deque>
#include <cstring> //memcpy/memset

namespace gnuradio
{

struct InputBufferQueues
{
    ~InputBufferQueues(void)
    {
        this->resize(0);
    }

    void init(
        const std::vector<size_t> &input_history_items,
        const std::vector<size_t> &input_multiple_items,
        const std::vector<size_t> &input_item_sizes
    );

    //! Call to get an input buffer for work
    SBuffer front(const size_t i, bool &potential_inline);

    //! Call when input bytes consumed by work
    void consume(const size_t i, const size_t bytes_consumed);

    void resize(const size_t size);

    inline void push(const size_t i, const SBuffer &buffer)
    {
        _queues[i].push_back(buffer);
        _enqueued_bytes[i] += _queues[i].back().length;
        __update(i);
    }

    inline void flush(const size_t i)
    {
        _queues[i] = std::deque<SBuffer>();
        _bitset.reset(i);
    }

    size_t size(void) const
    {
        return _queues.size();
    }

    inline void flush_all(void)
    {
        const size_t old_size = this->size();
        this->resize(0);
        this->resize(old_size);
    }

    inline bool ready(const size_t i) const
    {
        return _bitset[i];
    }

    inline bool empty(const size_t i) const
    {
        return not _bitset[i];
    }

    inline bool all_ready(void) const
    {
        return (~_bitset).none();
    }

    void __prepare(const size_t i);

    inline void __update(const size_t i)
    {
        _bitset.set(i, _enqueued_bytes[i] >= _reserve_bytes[i]);
    }

    boost::dynamic_bitset<> _bitset;
    std::vector<size_t> _enqueued_bytes;
    std::vector<std::deque<SBuffer> > _queues;
    std::vector<size_t> _history_bytes;
    std::vector<size_t> _reserve_bytes;
    std::vector<size_t> _multiple_bytes;
    std::vector<size_t> _post_bytes;
    std::vector<boost::shared_ptr<BufferQueue> > _aux_queues;
};


inline void InputBufferQueues::resize(const size_t size)
{
    _bitset.resize(size);
    _enqueued_bytes.resize(size, 0);
    _queues.resize(size);
    _history_bytes.resize(size, 0);
    _reserve_bytes.resize(size, 0);
    _multiple_bytes.resize(size, 0);
    _post_bytes.resize(size, 0);
    _aux_queues.resize(size);
}

static size_t round_up_to_multiple(const size_t at_least, const size_t multiple)
{
    size_t result = (multiple*at_least)/multiple;
    while (result < at_least) result += multiple;
    ASSERT((multiple*result)/multiple == result);
    return result;
}


inline void InputBufferQueues::init(
    const std::vector<size_t> &input_history_items,
    const std::vector<size_t> &input_multiple_items,
    const std::vector<size_t> &input_item_sizes
){
    if (this->size() == 0) return;

    const size_t max_history_items = *std::max_element(input_history_items.begin(), input_history_items.end());

    for (size_t i = 0; i < this->size(); i++)
    {
        ASSERT(input_multiple_items[i] > 0);

        _aux_queues[i] = boost::shared_ptr<BufferQueue>(new BufferQueue());

        //determine byte sizes for buffers and dealing with history
        const size_t old_history = _history_bytes[i];
        _history_bytes[i] = input_item_sizes[i]*input_history_items[i];

        //calculate the input multiple aka reserve size
        _multiple_bytes[i] = input_item_sizes[i]*input_multiple_items[i];
        _multiple_bytes[i] = std::max(size_t(1), _multiple_bytes[i]);

        //calculate the input multiple aka reserve size
        _reserve_bytes[i] = round_up_to_multiple(
            _history_bytes[i] + _multiple_bytes[i],
            _multiple_bytes[i]
        );

        //post bytes are the desired buffer size to escape the edge case
        _post_bytes[i] = round_up_to_multiple(
            input_item_sizes[i]*max_history_items + _reserve_bytes[i],
            _multiple_bytes[i]
        );

        //allocate mini buffers for history edge conditions
        size_t num_bytes = _post_bytes[i];
        _aux_queues[i]->allocate_one(num_bytes);
        _aux_queues[i]->allocate_one(num_bytes);

        //there is history, so enqueue some initial history
        if (_history_bytes[i] > old_history)
        {
            SBuffer buff = _aux_queues[i]->front();
            _aux_queues[i]->pop();

            const size_t delta = _history_bytes[i] - old_history;
            std::memset(buff.get_actual_memory(), 0, delta);
            buff.offset = 0;
            buff.length = delta;

            this->push(i, buff);
        }
        if (_history_bytes[i] < old_history)
        {
            size_t delta = old_history - _history_bytes[i];
            delta = std::min(delta, _enqueued_bytes[i]); //FIXME
            //TODO consume extra delta on push...? so we dont need std::min
            this->consume(i, delta);
        }
    }
}


inline SBuffer InputBufferQueues::front(const size_t i, bool &potential_inline)
{
    //if (_queues[i].empty()) return BuffInfo();

    ASSERT(not _queues[i].empty());
    ASSERT(this->ready(i));
    __prepare(i);
    ASSERT(_queues[i].front().length >= _history_bytes[i]);
    SBuffer &front = _queues[i].front();
    const bool unique = front.unique();

    //same buffer, different offset and length
    SBuffer buff = front;
    buff.length -= _history_bytes[i];
    buff.length /= _multiple_bytes[i];
    buff.length *= _multiple_bytes[i];

    //set the flag that this buffer *might* be inlined as an output buffer
    potential_inline = unique and (buff.length == front.length);

    return buff;
}

inline void InputBufferQueues::__prepare(const size_t i)
{
    //HERE();
    //assumes that we are always pushing proper history buffs on front
    ASSERT(_queues[i].front().length >= _history_bytes[i]);

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
}


inline void InputBufferQueues::consume(const size_t i, const size_t bytes_consumed)
{
    //if (bytes_consumed == 0) return true;

    //assert that we dont consume past the bounds of the buffer
    ASSERT(_queues[i].front().length >= bytes_consumed);

    //update bounds on the current buffer
    _queues[i].front().offset += bytes_consumed;
    _queues[i].front().length -= bytes_consumed;

    //safe to pop here when the buffer is consumed and no history
    if (_queues[i].front().length == 0 and _history_bytes[i] == 0)
    {
        _queues[i].pop_front();
    }

    //update the number of bytes in this queue
    ASSERT(_enqueued_bytes[i] >= bytes_consumed);
    _enqueued_bytes[i] -= bytes_consumed;
    __update(i);
}

} //namespace gnuradio

#endif /*INCLUDED_LIBGRAS_IMPL_INPUT_BUFFERS_HPP*/
