// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_LIBGRAS_IMPL_SIMPLE_BUFFER_QUEUE_HPP
#define INCLUDED_LIBGRAS_IMPL_SIMPLE_BUFFER_QUEUE_HPP

#include <gras/sbuffer.hpp>
#include <boost/bind.hpp>
#include <queue>

namespace gras
{

struct SimpleBufferQueue : std::queue<SBuffer>
{
    SimpleBufferQueue(void)
    {
        SBufferDeleter deleter = boost::bind(&SimpleBufferQueue::push_back, this, _1);
        _token = SBufferToken(new SBufferDeleter(deleter));
    }

    ~SimpleBufferQueue(void)
    {
        _token.reset();
        while (not this->empty())
        {
            this->pop();
        }
    }

    void push_back(const SBuffer &buff)
    {
        this->push(buff);
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

} //namespace gras

#endif /*INCLUDED_LIBGRAS_IMPL_SIMPLE_BUFFER_QUEUE_HPP*/
