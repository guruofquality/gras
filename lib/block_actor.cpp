// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/thread_pool.hpp>
#include <gras_impl/block_actor.hpp>
#include <Theron/Framework.h>
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

BlockActor::BlockActor(const ThreadPool &tp):
    Theron::Actor((tp)? *tp : *get_active_thread_pool())
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

BlockActor::~BlockActor(void)
{
    //NOP
}
