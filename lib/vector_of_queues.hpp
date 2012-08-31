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

#ifndef INCLUDED_LIBGNURADIO_VECTOR_OF_QUEUES_HPP
#define INCLUDED_LIBGNURADIO_VECTOR_OF_QUEUES_HPP

#include <boost/dynamic_bitset.hpp>
#include <vector>
#include <queue>

namespace gnuradio
{

template <typename T>
struct VectorOfQueues
{
    boost::dynamic_bitset<> _bitset;
    std::vector<std::queue<T> > _queues;

    inline void resize(const size_t size)
    {
        _bitset.resize(size);
        _queues.resize(size);
    }

    inline void push(const size_t i, const T &value)
    {
        _queues[i].push(value);
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
        _queues[i].pop();
        _bitset.set(i, not _queues[i].empty());
    }

    inline void flush(const size_t i)
    {
        _queues[i] = std::queue<T>();
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

#endif /*INCLUDED_LIBGNURADIO_VECTOR_OF_QUEUES_HPP*/
