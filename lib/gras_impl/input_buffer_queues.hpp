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
#include <tsbe/buffer.hpp>
#include <boost/dynamic_bitset.hpp>
#include <vector>
#include <deque>
#include <cstring> //memcpy/memset

namespace gnuradio
{

inline void my_buff_alloc(tsbe::Buffer &buff, const size_t num_bytes)
{
    if (num_bytes == 0)
    {
        buff = tsbe::Buffer(); //empty
        return;
    }
    if (not buff or buff.get_length() != num_bytes)
    {
        tsbe::BufferConfig config;
        config.memory = NULL;
        config.length = num_bytes;
        buff = tsbe::Buffer(config);
        std::memset(buff.get_memory(), 0, buff.get_length());
    }
}

struct BufferWOffset
{
    BufferWOffset(void): offset(0){}
    BufferWOffset(const tsbe::Buffer &buffer):
        offset(0), buffer(buffer){}
    size_t offset;
    tsbe::Buffer buffer;
};

struct BuffInfo
{
    void *mem;
    size_t len;
};

struct InputBufferQueues
{
    boost::dynamic_bitset<> _bitset;
    std::vector<std::deque<BufferWOffset> > _queues;
    std::vector<size_t> _history_bytes;
    std::vector<size_t> _post_bytes;
    std::vector<tsbe::Buffer> _history_buffs;

    std::vector<size_t> _reserve_bytes;
    std::vector<size_t> _enqueued_bytes;
    std::vector<tsbe::Buffer> _reserve_buffs;

    template <typename V>
    inline void init(
        const V &input_history_items,
        const V &input_item_sizes
    ){
        if (this->size() == 0) return;

        const size_t max_history_items = *std::max_element(input_history_items.begin(), input_history_items.end());

        for (size_t i = 0; i < this->size(); i++)
        {
            //determine byte sizes for buffers and dealing with history
            _history_bytes[i] = input_item_sizes[i]*input_history_items[i];
            _post_bytes[i] = input_item_sizes[i]*max_history_items;

            //allocate mini buffer for history edge conditions
            const size_t num_bytes = _history_bytes[i] + _post_bytes[i];
            my_buff_alloc(_history_buffs[i], num_bytes);
        }
    }

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
    inline BuffInfo front(const size_t i)
    {
        __prepare_front(i);

        BuffInfo info;
        const BufferWOffset &bo = _queues[i].front();
        const tsbe::Buffer &buff = bo.buffer;

        if (bo.offset < _history_bytes[i])
        {
            const tsbe::Buffer &hist_buff = _history_buffs[i];
            ASSERT(buff.get_length() >= _post_bytes[i]);

            if (bo.offset == 0)
            {
                char *src_mem = ((char *)buff.get_memory());
                char *dst_mem = ((char *)hist_buff.get_memory()) + _history_bytes[i];
                std::memcpy(dst_mem, src_mem, _post_bytes[i]);
            }

            info.mem = (char *)hist_buff.get_memory() + bo.offset;
            info.len = hist_buff.get_length() - bo.offset - _history_bytes[i];
        }

        else
        {
            info.mem = ((char *)buff.get_memory()) + bo.offset - _history_bytes[i];
            info.len = buff.get_length() - bo.offset;
        }

        if (_reserve_bytes[i] != 0)
        {
            info.len /= _reserve_bytes[i];
            info.len *= _reserve_bytes[i];
        }

        return info;
    }

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
    inline bool consume(const size_t i, const size_t bytes_consumed)
    {
        BufferWOffset &bo = _queues[i].front();
        const tsbe::Buffer &buff = bo.buffer;

        __consume(i, bytes_consumed);
        bo.offset += bytes_consumed;

        //if totally consumed, memcpy history and pop
        if (bo.offset >= buff.get_length())
        {
            ASSERT(bo.offset == buff.get_length()); //bad to consume more than buffer allows
            /*if (bo.offset != buff.get_length())
            {
                VAR(bo.offset);
                VAR(buff.get_length());
            }*/
            if (_history_bytes[i] != 0)
            {
                char *src_mem = ((char *)buff.get_memory()) + bo.offset - _history_bytes[i];
                std::memcpy(_history_buffs[i].get_memory(), src_mem, _history_bytes[i]);
            }
            __pop(i);
            return true;
        }
        return _history_bytes[i] == 0;
    }

    inline void resize(const size_t size)
    {
        _bitset.resize(size);
        _queues.resize(size);
        _post_bytes.resize(size, 0);
        _history_bytes.resize(size, 0);
        _history_buffs.resize(size);

        _reserve_buffs.resize(size);
        _reserve_bytes.resize(size, 0);
        _enqueued_bytes.resize(size, 0);
    }

    inline void set_reserve(const size_t i, const size_t num_bytes)
    {
        //TODO, we may call this dynamically, so 1) call __update
        //and 2) safely resize the buffer and preserve its data
        /*if (num_bytes)
        {
            std::cout << "set_reserve " << i << " " << num_bytes << " bytes\n";
        }//*/
        _reserve_bytes[i] = num_bytes;
        my_buff_alloc(_reserve_buffs[i], _reserve_bytes[i]);
        if (_reserve_buffs[i]) _reserve_buffs[i].get_length() = 0;
    }

    inline void push(const size_t i, const tsbe::Buffer &buffer)
    {
        _queues[i].push_back(buffer);
        _enqueued_bytes[i] += buffer.get_length();
        __update(i);
    }

    inline void __prepare_front(const size_t i)
    {
        VAR(_reserve_bytes[i]);
        VAR(_history_bytes[i]);
        ASSERT(_history_bytes[i] == 0 or _reserve_bytes[i] == 0); //FIXME dont mix history and reserve for now
        tsbe::Buffer &reserve_buff = _reserve_buffs[i];

        {
            BufferWOffset &bo = _queues[i].front();
            const tsbe::Buffer &buff = bo.buffer;

            //the buffer has enough space to meet reserve reqs as-is
            if (buff.get_length() >= _reserve_bytes[i] + bo.offset)
            {
                return;
            }

            //if its already the reserve buff, memmove that shit
            if (buff.get() == reserve_buff.get())
            {
                const size_t bytes_left = buff.get_length() - bo.offset;
                std::memmove(
                    reserve_buff.get_memory(),
                    ((char *)buff.get_memory()) + bo.offset,
                    bytes_left
                );
                reserve_buff.get_length() = bytes_left;
                __pop(i);
            }
        }

        //now we have a state where the front buff is not reserve
        while (reserve_buff.get_length() < _reserve_bytes[i])
        {
            BufferWOffset &bo = _queues[i].front();
            const size_t bytes_to_copy = std::min(
                bo.buffer.get_length()-bo.offset,
                _reserve_bytes[i]-reserve_buff.get_length()
            );
            std::memcpy(
                ((char *)reserve_buff.get_memory())+reserve_buff.get_length(),
                ((char *)bo.buffer.get_memory())+bo.offset,
                bytes_to_copy
            );
            reserve_buff.get_length() += bytes_to_copy;
            bo.offset += bytes_to_copy;
            if (bo.offset >= bo.buffer.get_length()) __pop(i);
        }
        _queues[i].push_front(reserve_buff);
    }

    inline void __consume(const size_t i, const size_t num_bytes)
    {
        ASSERT(_enqueued_bytes[i] >= num_bytes);
        _enqueued_bytes[i] -= num_bytes;
        __update(i);
    }

    inline void __pop(const size_t i)
    {
        //TODO FIXME quick hack
        if (_queues[i].front().buffer.get() == _reserve_buffs[i].get())
        {
            _reserve_buffs[i].get_length() = 0;
        }
        _queues[i].pop_front();
    }

    inline void __update(const size_t i)
    {
        _bitset.set(i, _enqueued_bytes[i] >= _reserve_bytes[i] and _enqueued_bytes[i] > 0);
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
};

} //namespace gnuradio

#endif /*INCLUDED_LIBGRAS_IMPL_INPUT_BUFFERS_HPP*/
