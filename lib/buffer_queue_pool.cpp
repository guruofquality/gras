// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/buffer_queue.hpp>
#include <gras_impl/debug.hpp>
#include <boost/circular_buffer.hpp>

using namespace gras;

struct BufferQueuePool : BufferQueue
{
    BufferQueuePool(const SBufferConfig &config, const size_t num):
        token(config.token), //save config, its holds token
        queue(boost::circular_buffer<SBuffer>(num))
    {
        //NOP
    }

    ~BufferQueuePool(void)
    {
        token.reset();
        queue.clear();
    }

    SBuffer &front(void)
    {
        ASSERT(not queue.empty());
        ASSERT(queue.front());
        return queue.front();
    }

    void pop(void)
    {
        ASSERT(not queue.empty());
        queue.front().reset(); //dont hold ref
        queue.pop_front();
    }

    void push(const SBuffer &buff)
    {
        queue.push_back(buff);
    }

    bool empty(void) const
    {
        return queue.empty();
    }

    SBufferToken token;
    boost::circular_buffer<SBuffer> queue;

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
        //bq->push(buff);
    }

    return queue;
}
