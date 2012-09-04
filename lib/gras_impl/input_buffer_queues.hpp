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
#include <queue>
#include <cstring> //memcpy/memset

namespace gnuradio
{

struct BuffInfo
{
    void *mem;
    size_t len;
};

struct InputBufferQueues
{
    boost::dynamic_bitset<> _bitset;
    std::vector<std::queue<tsbe::Buffer> > _queues;
    std::vector<size_t> _offset_bytes;
    std::vector<size_t> _history_bytes;
    std::vector<size_t> _post_bytes;
    std::vector<tsbe::Buffer> _history_buffs;

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
            tsbe::Buffer &buff = _history_buffs[i];
            if (not buff or buff.get_length() != num_bytes)
            {
                tsbe::BufferConfig config;
                config.memory = NULL;
                config.length = num_bytes;
                buff = tsbe::Buffer(config);
                std::memset(buff.get_memory(), 0, buff.get_length());
            }
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
    inline BuffInfo front(const size_t i) const
    {
        BuffInfo info;
        const tsbe::Buffer &buff = _queues[i].front();

        if (_offset_bytes[i] < _history_bytes[i])
        {
            const tsbe::Buffer &hist_buff = _history_buffs[i];
            ASSERT(buff.get_length() >= _post_bytes[i]);

            if (_offset_bytes[i] == 0)
            {
                char *src_mem = ((char *)buff.get_memory());
                char *dst_mem = ((char *)hist_buff.get_memory()) + _history_bytes[i];
                std::memcpy(dst_mem, src_mem, _post_bytes[i]);
            }

            info.mem = (char *)hist_buff.get_memory() + _offset_bytes[i];
            info.len = hist_buff.get_length() - _offset_bytes[i] - _history_bytes[i];
        }

        else
        {
            info.mem = ((char *)buff.get_memory()) + _offset_bytes[i] - _history_bytes[i];
            info.len = buff.get_length() - _offset_bytes[i];
        }

        return info;
    }

    /*!
     * Rules for popping:
     *
     * If we were operating in a mini history buffer, do nothing.
     * Otherwise, check if the input buffer was entirely consumed.
     * If so, pop the input buffer, copy the tail end of the buffer
     * into the mini history buffer, and reset the offset condition.
     *
     * \return true if the input allows output flushing
     */
    inline bool pop(const size_t i, const size_t bytes_consumed)
    {
        _offset_bytes[i] += bytes_consumed;

        //if totally consumed, memcpy history and pop
        const tsbe::Buffer &buff = _queues[i].front();
        if (_offset_bytes[i] >= buff.get_length())
        {
            ASSERT(_offset_bytes[i] == buff.get_length()); //bad to consume more than buffer allows
            /*if (_offset_bytes[i] != buff.get_length())
            {
                VAR(_offset_bytes[i]);
                VAR(buff.get_length());
            }*/
            if (_history_bytes[i] != 0)
            {
                char *src_mem = ((char *)buff.get_memory()) + _offset_bytes[i] - _history_bytes[i];
                std::memcpy(_history_buffs[i].get_memory(), src_mem, _history_bytes[i]);
            }
            _queues[i].pop();
            _bitset.set(i, not _queues[i].empty());
            _offset_bytes[i] = 0;
            return true;
        }
        return _history_bytes[i] == 0;
    }

    inline void resize(const size_t size)
    {
        _bitset.resize(size);
        _queues.resize(size);
        _post_bytes.resize(size, 0);
        _offset_bytes.resize(size, 0);
        _history_bytes.resize(size, 0);
        _history_buffs.resize(size);
    }

    inline void push(const size_t i, const tsbe::Buffer &value)
    {
        _queues[i].push(value);
        _bitset.set(i);
    }

    inline void flush(const size_t i)
    {
        _queues[i] = std::queue<tsbe::Buffer>();
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
        return not _queues[i].empty();
    }

    inline bool empty(const size_t i) const
    {
        return _queues[i].empty();
    }

    inline bool all_ready(void) const
    {
        return (~_bitset).none();
    }
};

} //namespace gnuradio

#endif /*INCLUDED_LIBGRAS_IMPL_INPUT_BUFFERS_HPP*/
