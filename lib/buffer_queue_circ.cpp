// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/buffer_queue.hpp>
#include <gras_impl/debug.hpp>
#include <boost/circular_buffer.hpp>
#include <vector>

using namespace gras;

SBuffer make_circular_buffer(const size_t num_bytes); //circular_buffer.cpp

static void buffer_dummy_delete(SBuffer &, const SBuffer /*ref holder*/)
{
    //NOP
}

struct BufferQueueCirc : BufferQueue
{
    BufferQueueCirc(const SBufferConfig &config, const size_t num);

    ~BufferQueueCirc(void)
    {
        _token.reset();
        _available_buffers.clear();
        _returned_buffers.clear();
    }

    SBuffer &front(void);

    void pop(void);

    void push(const SBuffer &buff);

    bool empty(void) const
    {
        return _bytes_avail == 0 or _available_buffers.empty();
    }

    SBufferToken _token;
    SBuffer _circ_buff;
    char *_write_ptr;
    char *_last_ptr;
    size_t _bytes_avail;
    size_t _ack_index;
    boost::circular_buffer<SBuffer> _available_buffers;
    std::vector<SBuffer> _returned_buffers;
    std::vector<size_t> _outgone_bytes;

};

BufferQueueCirc::BufferQueueCirc(const SBufferConfig &config, const size_t num_buffs):
    _token(config.token),
    _write_ptr(NULL),
    _last_ptr(NULL),
    _ack_index(0)
{
    //allocate a large buffer
    const size_t num_bytes = config.length * num_buffs;
    _circ_buff = make_circular_buffer(num_bytes);
    _write_ptr = (char *)_circ_buff.get_actual_memory();
    _bytes_avail = _circ_buff.get_actual_length();

    //create dummy deleter to hold ref to circ buff
    SBufferDeleter deleter = boost::bind(&buffer_dummy_delete, _1, _circ_buff);

    //allocate pool of sbuffers
    _available_buffers.set_capacity(num_buffs);
    _returned_buffers.resize(num_buffs);
    _outgone_bytes.resize(num_buffs, 0);
    SBufferConfig sconfig = config;
    sconfig.deleter = deleter;
    sconfig.memory = _circ_buff.get_actual_memory();
    for (size_t i = 0; i < num_buffs; i++)
    {
        sconfig.user_index = i;
        SBuffer buff(sconfig);
        //buffer derefs and returns to this queue thru token callback
    }
}

SBuffer &BufferQueueCirc::front(void)
{
    ASSERT(not this->empty());
    SBuffer &front = _available_buffers.front();
    ASSERT(front);
    front->config.memory = _write_ptr;
    front.last = _last_ptr;
    ASSERT(front.offset == 0);
    if (_last_ptr == _write_ptr) ASSERT(front.get() == front.last);
    return front;
}

void BufferQueueCirc::pop(void)
{
    ASSERT(not this->empty());
    SBuffer &front = _available_buffers.front();
    ASSERT(front.length == 0);
    const size_t num_bytes = front.offset;

    //store number of bytes for buffer return
    _outgone_bytes[front.get_user_index()] = num_bytes;

    //pop the buffer from internal reference
    _available_buffers.pop_front();

    //adjust the write pointer
    _write_ptr += num_bytes;
    _last_ptr = _write_ptr;

    //handle circular wrap
    if (_write_ptr > (char *)_circ_buff.get(_circ_buff.get_actual_length()))
    {
        _write_ptr -= _circ_buff.get_actual_length();
    }

    //subtract out of available bytes
    ASSERT(_bytes_avail >= num_bytes);
    _bytes_avail -= num_bytes;
}

void BufferQueueCirc::push(const SBuffer &buff)
{
    //is it my buffer? otherwise dont keep it
    if GRAS_UNLIKELY(buff->config.token.lock() != _token) return;

    ASSERT(buff.get_user_index() < _returned_buffers.size());
    _returned_buffers[buff.get_user_index()] = buff;

    //ack starting at the expected index and up
    while (_returned_buffers[_ack_index])
    {
        //return the held bytes to the available
        _bytes_avail += _outgone_bytes[_ack_index];

        //remove the buffer container into the queue
        _available_buffers.push_back(_returned_buffers[_ack_index]);
        _returned_buffers[_ack_index].reset();

        //increment the ack index for the next run
        if (++_ack_index == _returned_buffers.size()) _ack_index = 0;
    }
}

BufferQueueSptr BufferQueue::make_circ(
    const SBufferConfig &config,
    const size_t num_buffs
){
    return BufferQueueSptr(new BufferQueueCirc(config, num_buffs));
}
