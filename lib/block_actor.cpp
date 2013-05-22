// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/thread_pool.hpp>
#include <gras_impl/block_actor.hpp>
#include <Theron/Framework.h>
#include <Theron/Receiver.h>
#include <iostream>

using namespace gras;

/***********************************************************************
 * Active framework implementation
 **********************************************************************/
static boost::weak_ptr<Theron::Framework> weak_framework;

void ThreadPool::set_active(void)
{
    weak_framework = *this;
}

static ThreadPool active_thread_pool;

ThreadPool get_active_thread_pool(void)
{
    if (not weak_framework.lock())
    {
        active_thread_pool = ThreadPool(ThreadPoolConfig());
        active_thread_pool.set_active();
        std::cout << "Created default thread pool with " << active_thread_pool->GetNumThreads() << " threads." << std::endl;
    }
    return weak_framework;
}

/***********************************************************************
 * Block actor construction - gets active framework
 **********************************************************************/

BlockActor::BlockActor(void):
    Apology::Worker(*get_active_thread_pool())
{
    const char * gras_tpp = getenv("GRAS_TPP");
    if (gras_tpp != NULL)
    {
        ThreadPoolConfig config;
        config.thread_count = 1;
        this->thread_pool = ThreadPool(config);
    }
    else
    {
        this->thread_pool = get_active_thread_pool();
        active_thread_pool.reset(); //actors hold this, now its safe to reset, weak_framework only
    }
    this->register_handlers();

}

void BlockActor::ping_thread_loop(void)
{
    Theron::Receiver receiver;
    while (_ping_thread_running)
    {
        this->Send(PingMessage(), receiver.GetAddress());
        const boost::system_time sent_time = boost::get_system_time();
        boost::this_thread::sleep(boost::posix_time::seconds(1));
        while (receiver.Count() != 1)
        {
            std::cerr
                << boost::format("Deadlock warning: %s has not yielded the thread context for %u seconds.")
                % block_ptr->to_string() % (boost::get_system_time() - sent_time).total_seconds()
                << std::endl;
            boost::this_thread::sleep(boost::posix_time::seconds(1));
        }
        receiver.Consume(1);
    }
}

void BlockActor::handle_ping(const PingMessage &, const Theron::Address from)
{
    this->Send(0, from); //ACK
}

BlockActor::~BlockActor(void)
{
    
}
