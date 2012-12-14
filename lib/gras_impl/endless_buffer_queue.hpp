// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_LIBGRAS_IMPL_ENDLESS_BUFFER_QUEUE_HPP
#define INCLUDED_LIBGRAS_IMPL_ENDLESS_BUFFER_QUEUE_HPP

#include <gras_impl/debug.hpp>
#include <gras/sbuffer.hpp>
#include <boost/bind.hpp>
#include <boost/circular_buffer.hpp>

namespace gras
{

struct EndlessBufferQueue
{
    enum {MAX_QUEUE_SIZE = 128};

    static SBuffer make_circular_buffer(const size_t num_bytes);

    EndlessBufferQueue(const size_t num_bytes);

    SBuffer &front(void);

    void pop(const size_t num_bytes);

    void push(const SBuffer &buff);

    SBufferToken _token;
    SBuffer _circ_buff;
    char *_write_ptr;
    size_t _bytes_avail;
    size_t _cur_index, _ack_index;
    boost::circular_buffer<SBuffer> _available_buffers;
    boost::circular_buffer<SBuffer> _returned_buffers;
};

EndlessBufferQueue::EndlessBufferQueue(const size_t num_bytes)
{
    _cur_index = 0;
    _ack_index = 0;

    //allocate a large buffer
    _circ_buff = EndlessBufferQueue::make_circular_buffer(num_bytes);
    _write_ptr = (char *)_circ_buff.get_actual_memory();
    _bytes_avail = _circ_buff.get_actual_length();

    //create token as buffer returner
    SBufferDeleter deleter = boost::bind(&EndlessBufferQueue::push, this, _1);
    _token = SBufferToken(new SBufferDeleter(deleter));

    //allocate pool of sbuffers
    _available_buffers.resize(MAX_QUEUE_SIZE);
    _returned_buffers.resize(MAX_QUEUE_SIZE);
    SBufferConfig config;
    config.memory = _circ_buff.get_actual_memory();
    config.length = _circ_buff.get_actual_length();
    for (size_t i = 0; i < _available_buffers.size(); i++)
    {
        _available_buffers.push_back(SBuffer(config));
    }
}

GRAS_FORCE_INLINE SBuffer &EndlessBufferQueue::front(void)
{
    ASSERT(_bytes_avail);
    ASSERT(not _available_buffers.empty());
    SBuffer &front = _available_buffers.front();
    front->config.memory = _write_ptr;
    front->config.length = _bytes_avail;
    front.offset = 0;
    front.length = 0;
    return front;
}

GRAS_FORCE_INLINE void EndlessBufferQueue::pop(const size_t num_bytes)
{
    ASSERT(_bytes_avail >= num_bytes);
    SBuffer &front = _available_buffers.front();
    front->config.length = num_bytes;
    front->config.user_index = _cur_index++;
    _write_ptr += num_bytes;
    if (_write_ptr > (char *)_circ_buff.get(circ_buff.get_actual_length()))
    {
        _write_ptr -= circ_buff.get_actual_length();
    }
    _bytes_avail -= num_bytes;
}

void EndlessBufferQueue::push(const SBuffer &buff)
{
    _returned_buffers.push_back(buff);
    //BOOST_FOREACH(
    //TODO update available
}

} //namespace gras

#endif /*INCLUDED_LIBGRAS_IMPL_ENDLESS_BUFFER_QUEUE_HPP*/
