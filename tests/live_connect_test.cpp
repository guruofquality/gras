// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <boost/test/unit_test.hpp>
#include <stdexcept>
#include <iostream>

#include <gras/block.hpp>
#include <gras/top_block.hpp>

#include <boost/thread/thread.hpp>

struct MySource : gras::Block
{
    MySource(void):
        gras::Block("MySource")
    {
        this->output_config(0).item_size = 8;
    }

    void work(const InputItems &, const OutputItems &outs)
    {
        std::memset(outs[0].cast<void *>(), 0, outs[0].size()*8);
        this->produce(outs[0].size());
    }
};

struct MySink : gras::Block
{
    MySink(const bool use_pool = true):
        gras::Block("MySink"),
        use_pool(use_pool)
    {
        this->input_config(0).item_size = 8;
    }

    void work(const InputItems &ins, const OutputItems &)
    {
        for (size_t i = 0; i < ins[0].size()*8; i++)
        {
            if (ins[0].cast<const char *>()[i] != 0)
            {
                throw std::runtime_error("bad memory -> should be zero");
            }
        }
        this->consume(ins[0].size());
    }

    gras::BufferQueueSptr input_buffer_allocator(const size_t, const gras::SBufferConfig &config)
    {
        if (use_pool) return gras::BufferQueue::make_pool(config, 4);
        return gras::BufferQueue::make_circ(config, 4);
    }

    const bool use_pool;
};

static void sleep_rand(void)
{
    boost::this_thread::sleep(boost::posix_time::milliseconds(std::rand()%10 + 10));
}

BOOST_AUTO_TEST_CASE(test_live_connect)
{
    MySink my_sink(false);
    MySource my_source;
    gras::TopBlock tb("Top");

    tb.connect(my_source, 0, my_sink, 0);
    tb.start();

    //new connection and commit it
    for (size_t i = 0; i < 50; i++)
    {
        const bool use_pool_allocator = bool(std::rand()%2);
        MySink my_sink_tmp(use_pool_allocator);
        tb.connect(my_source, 0, my_sink_tmp, 0);
        tb.commit();
        sleep_rand();
        tb.disconnect(my_source, 0, my_sink_tmp, 0);
        tb.commit();
        BOOST_CHECK(my_sink_tmp.get_consumed(0) > 0);
    }

    tb.stop();
    tb.wait();
}
