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

#ifndef INCLUDED_LIBGRAS_IMPL_BUFFER_QUEUE_HPP
#define INCLUDED_LIBGRAS_IMPL_BUFFER_QUEUE_HPP

#include <gnuradio/sbuffer.hpp>
#include <boost/bind.hpp>
#include <queue>

namespace gnuradio
{

struct BufferQueue : std::queue<SBuffer>
{
    BufferQueue(void)
    {
        SBufferDeleter deleter = boost::bind(&BufferQueue::push, this, _1);
        _token = SBufferToken(new SBufferDeleter(deleter));
    }

    ~BufferQueue(void)
    {
        _token.reset();
        while (not this->empty())
        {
            this->pop();
        }
    }

    void allocate_one(const size_t num_bytes)
    {
        SBufferConfig config;
        config.memory = NULL;
        config.length = num_bytes;
        config.token = _token;
        SBuffer buff(config);
        //buffer derefs here and the token messages it back to the queue
    }

    SBufferToken _token;
};

} //namespace gnuradio

#endif /*INCLUDED_LIBGRAS_IMPL_BUFFER_QUEUE_HPP*/
