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

#ifndef INCLUDED_LIBGRAS_IMPL_OUTPUT_BUFFER_QUEUES_HPP
#define INCLUDED_LIBGRAS_IMPL_OUTPUT_BUFFER_QUEUES_HPP

#include <gras_impl/bitset.hpp>
#include <vector>
#include <boost/circular_buffer.hpp>

namespace gras
{

template <typename T>
struct OutputBufferQueues
{
    enum {MAX_QUEUE_SIZE = 128};

    BitSet _bitset;
    std::vector<boost::circular_buffer<T> > _queues;

    GRAS_FORCE_INLINE void resize(const size_t size)
    {
        _bitset.resize(size);
        _queues.resize(size, boost::circular_buffer<T>(MAX_QUEUE_SIZE));
    }

    GRAS_FORCE_INLINE void push(const size_t i, const T &value)
    {
        _queues[i].push_back(value);
        _bitset.set(i);
    }

    //! used for input buffer inlining
    GRAS_FORCE_INLINE void push_front(const size_t i, const T &value)
    {
        ASSERT(not _queues[i].full());
        _queues[i].push_front(value);
        _bitset.set(i);
    }

    GRAS_FORCE_INLINE const T &front(const size_t i) const
    {
        return _queues[i].front();
    }

    GRAS_FORCE_INLINE T &front(const size_t i)
    {
        return _queues[i].front();
    }

    GRAS_FORCE_INLINE void pop(const size_t i)
    {
        _queues[i].pop_front();
        _bitset.set(i, not _queues[i].empty());
    }

    GRAS_FORCE_INLINE void flush(const size_t i)
    {
        _queues[i].clear();
        _bitset.reset(i);
    }

    GRAS_FORCE_INLINE void flush_all(void)
    {
        for (size_t i = 0; i < this->size(); i++) this->flush(i);
    }

    GRAS_FORCE_INLINE bool ready(const size_t i) const
    {
        return not _queues[i].empty();
    }

    GRAS_FORCE_INLINE bool empty(const size_t i) const
    {
        return _queues[i].empty();
    }

    GRAS_FORCE_INLINE bool all_ready(void) const
    {
        return _bitset.all();
    }

    GRAS_FORCE_INLINE size_t size(void) const
    {
        return _queues.size();
    }
};

} //namespace gras

#endif /*INCLUDED_LIBGRAS_IMPL_OUTPUT_BUFFER_QUEUES_HPP*/
