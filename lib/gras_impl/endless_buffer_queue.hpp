// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_LIBGRAS_IMPL_ENDLESS_BUFFER_QUEUE_HPP
#define INCLUDED_LIBGRAS_IMPL_ENDLESS_BUFFER_QUEUE_HPP

#include <gras_impl/debug.hpp>
#include <gras/sbuffer.hpp>
#include <boost/bind.hpp>
#include <boost/circular_buffer.hpp>
#include <vector>

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

    GRAS_FORCE_INLINE bool empty(void) const
    {
        return _bytes_avail == 0 or _available_buffers.empty();
    }

    SBufferToken _token;
    SBuffer _circ_buff;
    char *_write_ptr;
    size_t _bytes_avail;
    size_t _ack_index;
    boost::circular_buffer<SBuffer> _available_buffers;
    std::vector<SBuffer> _returned_buffers;
};

EndlessBufferQueue::EndlessBufferQueue(const size_t num_bytes)
{
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
        config.user_index = i;
        _available_buffers.push_back(SBuffer(config));
    }
}

GRAS_FORCE_INLINE SBuffer &EndlessBufferQueue::front(void)
{
    ASSERT(not this->empty());
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
    _write_ptr += num_bytes;
    if (_write_ptr > (char *)_circ_buff.get(_circ_buff.get_actual_length()))
    {
        _write_ptr -= _circ_buff.get_actual_length();
    }
    _bytes_avail -= num_bytes;
}

void EndlessBufferQueue::push(const SBuffer &buff)
{
    _returned_buffers[buff.get_user_index()] = buff;
    while (_returned_buffers[_ack_index])
    {
        _available_buffers.push_back(_returned_buffers[_ack_index]);
        _returned_buffers[_ack_index].reset();
        _ack_index++;
        if (_ack_index == _returned_buffers.size()) _ack_index = 0;
    }
}

} //namespace gras

#endif /*INCLUDED_LIBGRAS_IMPL_ENDLESS_BUFFER_QUEUE_HPP*/
