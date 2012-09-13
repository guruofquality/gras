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

#ifndef INCLUDED_LIBGRAS_IMPL_OUTPUT_BUFFER_QUEUES_HPP
#define INCLUDED_LIBGRAS_IMPL_OUTPUT_BUFFER_QUEUES_HPP

#include <boost/dynamic_bitset.hpp>
#include <vector>
#include <deque>

namespace gnuradio
{

template <typename T>
struct OutputBufferQueues
{
    boost::dynamic_bitset<> _bitset;
    std::vector<std::deque<T> > _queues;

    inline void resize(const size_t size)
    {
        _bitset.resize(size);
        _queues.resize(size);
    }

    inline void push(const size_t i, const T &value)
    {
        _queues[i].push_back(value);
        _bitset.set(i);
    }

    //! used for input buffer inlining
    inline void push_front(const size_t i, const T &value)
    {
        _queues[i].push_front(value);
        _bitset.set(i);
    }

    inline const T &front(const size_t i) const
    {
        return _queues[i].front();
    }

    inline T &front(const size_t i)
    {
        return _queues[i].front();
    }

    inline void pop(const size_t i)
    {
        _queues[i].pop_front();
        _bitset.set(i, not _queues[i].empty());
    }

    inline void flush(const size_t i)
    {
        _queues[i] = std::deque<T>();
        _bitset.reset(i);
    }

    inline void flush_all(void)
    {
        _queues.clear();
        _queues.resize(_bitset.size());
        _bitset.reset();
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

#endif /*INCLUDED_LIBGRAS_IMPL_OUTPUT_BUFFER_QUEUES_HPP*/
