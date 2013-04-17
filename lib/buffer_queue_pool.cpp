// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/buffer_queue.hpp>
#include <gras_impl/debug.hpp>
#include <boost/circular_buffer.hpp>

using namespace gras;

struct BufferQueuePool : BufferQueue
{
    BufferQueuePool(const SBufferConfig &config, const size_t num):
        _token(config.token), //save config, its holds token
        _queue(boost::circular_buffer<SBuffer>(num))
    {
        //NOP
    }

    ~BufferQueuePool(void)
    {
        _token.reset();
        _queue.clear();
    }

    SBuffer &front(void)
    {
        ASSERT(not _queue.empty());
        ASSERT(_queue.front());
        return _queue.front();
    }

    void pop(void)
    {
        ASSERT(not _queue.empty());
        SBuffer &buff = _queue.front();

        //This little half consumed metric lets us keep using
        //the same buffer if its only been partially consumed.
        //Input buffer stitching will rejoin contiguous memory.
        if (buff.offset > buff.get_actual_length()/2)
        {
            buff.reset(); //dont hold ref
            _queue.pop_front();
        }
        else
        {
            //enables buffer stitching on pool buffers
            buff.last = buff.get();
        }
    }

    void push(const SBuffer &buff)
    {
        //is it my buffer? otherwise dont keep it
        if GRAS_UNLIKELY(buff->config.token.lock() != _token) return;

        //should never get a buffer from a circ queue
        ASSERT(buff.get_user_index() == size_t(~0));

        _queue.push_back(buff);
    }

    bool empty(void) const
    {
        return _queue.empty();
    }

    SBufferToken _token;
    boost::circular_buffer<SBuffer> _queue;

};

BufferQueueSptr BufferQueue::make_pool(
    const SBufferConfig &config,
    const size_t num_buffs
){
    BufferQueueSptr queue(new BufferQueuePool(config, num_buffs));

    for (size_t i = 0; i < num_buffs; i++)
    {
        SBuffer buff(config);
        std::memset(buff.get_actual_memory(), 0, buff.get_actual_length());
        //buffer derefs and returns to this queue thru token callback
    }

    return queue;
}
