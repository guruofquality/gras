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
#include <tsbe/buffer.hpp>
#include <boost/dynamic_bitset.hpp>
#include <vector>
#include <queue>
#include <deque>
#include <cstring> //memcpy/memset

namespace gnuradio
{

struct BufferWOffset
{
    BufferWOffset(void): offset(0), length(0){}
    BufferWOffset(const tsbe::Buffer &buffer):
        offset(0), length(buffer.get_length()), buffer(buffer){}

    inline char *mem_offset(void) const
    {
        return ((char *)buffer.get_memory()) + offset;
    }

    size_t offset;
    size_t length;
    tsbe::Buffer buffer;
};

struct BuffInfo
{
    void *mem;
    size_t len;
};

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

    /*!
     * Rules for front:
     *
     * If we are within the mini history buffer,
     * memcpy post bytes from the head of the input buffer.
     * The caller must chew through the mini history buffer
     * until offset bytes passes the history requirement.
     *
     * Otherwise, resolve pointers to the input buffer,
     * moving the memory and length by num history bytes.
     */
    BuffInfo front(const size_t i);

    /*!
     * Rules for consume:
     *
     * If we were operating in a mini history buffer, do nothing.
     * Otherwise, check if the input buffer was entirely consumed.
     * If so, pop the input buffer, copy the tail end of the buffer
     * into the mini history buffer, and reset the offset condition.
     *
     * \return true if the input allows output flushing
     */
    bool consume(const size_t i, const size_t bytes_consumed);

    void resize(const size_t size);

    inline void push(const size_t i, const tsbe::Buffer &buffer)
    {
        _queues[i].push_back(buffer);
        _enqueued_bytes[i] += buffer.get_length();
        __update(i);
    }

    inline void flush(const size_t i)
    {
        _queues[i] = std::deque<BufferWOffset>();
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
    std::vector<std::deque<BufferWOffset> > _queues;
    std::vector<size_t> _history_bytes;
    std::vector<size_t> _reserve_bytes;
    std::vector<BufferQueue> _aux_queues;
};


void InputBufferQueues::resize(const size_t size)
{
    _bitset.resize(size);
    _enqueued_bytes.resize(size, 0);
    _queues.resize(size);
    _history_bytes.resize(size, 0);
    _reserve_bytes.resize(size, 0);
    _aux_queues.resize(size);
}


void InputBufferQueues::init(
    const std::vector<size_t> &input_history_items,
    const std::vector<size_t> &input_multiple_items,
    const std::vector<size_t> &input_item_sizes
){
    if (this->size() == 0) return;

    const size_t max_history_items = *std::max_element(input_history_items.begin(), input_history_items.end());

    for (size_t i = 0; i < this->size(); i++)
    {
        //determine byte sizes for buffers and dealing with history
        _history_bytes[i] = input_item_sizes[i]*input_history_items[i];
        const size_t post_bytes = input_item_sizes[i]*max_history_items;
        const size_t byte_multiple = input_item_sizes[i]*input_multiple_items[i];
        _reserve_bytes[i] = byte_multiple;
        while (_reserve_bytes[i] < post_bytes)
        {
            _reserve_bytes[i] += byte_multiple;
        }

        //allocate mini buffers for history edge conditions
        const size_t num_bytes = _history_bytes[i] + _reserve_bytes[i];
        _aux_queues[i].allocate_one(num_bytes);
        _aux_queues[i].allocate_one(num_bytes);

        //there is history, so enqueue some initial history
        if (_history_bytes[i] != 0)
        {
            tsbe::Buffer buff = _aux_queues[i].front();
            _aux_queues[i].pop();

            std::memset(buff.get_memory(), 0, _history_bytes[i]);
            _queues[i].push_front(buff);
            _queues[i].front().length = _history_bytes[i];
        }
    }
}

inline BuffInfo InputBufferQueues::front(const size_t i)
{
    ASSERT(this->ready(i));
    __prepare(i);

    BufferWOffset &front = _queues[i].front();
    BuffInfo info;
    info.mem = front.mem_offset() - _history_bytes[i];
    info.len = front.length;
    info.len /= _reserve_bytes[i];
    info.len *= _reserve_bytes[i];
    return info;
}

inline void InputBufferQueues::__prepare(const size_t i)
{
    //this conditional statement is the requirement we must meet
    while (
        _queues[i].front().length < _reserve_bytes[i] or
        _queues[i].front().offset < _history_bytes[i]
    ){
        
    }
}

inline bool InputBufferQueues::consume(const size_t i, const size_t bytes_consumed)
{
    //update bounds on the current buffer
    _queues[i].front().offset += bytes_consumed;
    _queues[i].front().length -= bytes_consumed;

    //assert that we dont consum past the bounds of the buffer
    ASSERT(_queues[i].front().buffer.get_length() >= _queues[i].front().offset);

    //this input was completed, pop it free
    if (_queues[i].front().length == 0)
    {
        const BufferWOffset old_buff = _queues[i].front();
        _queues[i].pop_front();

        //push history into the front of the queue
        if (_history_bytes[i] != 0)
        {
            tsbe::Buffer buff = _aux_queues[i].front();
            _aux_queues[i].pop();

            const size_t hist_bytes = _history_bytes[i];
            std::memcpy(buff.get_memory(), old_buff.mem_offset() - hist_bytes, hist_bytes);
            _queues[i].push_front(buff);
            _queues[i].front().length = hist_bytes;
        }
    }

    //update the number of bytes in this queue
    ASSERT(_enqueued_bytes[i] >= bytes_consumed);
    _enqueued_bytes[i] -= bytes_consumed;
    __update(i);

    return true; //TODO not true on minibuff
}

} //namespace gnuradio

#endif /*INCLUDED_LIBGRAS_IMPL_INPUT_BUFFERS_HPP*/
