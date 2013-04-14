// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/thread_pool.hpp>
#include <gras_impl/block_actor.hpp>
#include <boost/thread/thread.hpp>
#include <Theron/Framework.h>
#include <stdexcept>

using namespace gras;

ThreadPoolConfig::ThreadPoolConfig(void)
{
    thread_count = boost::thread::hardware_concurrency();
    thread_count = std::max(size_t(2), thread_count);
    node_mask = 0;
    processor_mask = 0xffffffff;
    yield_strategy = "STRONG";

    //environment variable override
    const char * gras_yield = getenv("GRAS_YIELD");
    if (gras_yield != NULL) yield_strategy = gras_yield;
}

/***********************************************************************
 * Thread pool implementation
 **********************************************************************/
ThreadPool::ThreadPool(void)
{
    //NOP
}

ThreadPool::ThreadPool(boost::weak_ptr<Theron::Framework> p):
    boost::shared_ptr<Theron::Framework>(p.lock())
{
    //NOP
}

ThreadPool::ThreadPool(const ThreadPoolConfig &config)
{
    Theron::Framework::Parameters params(
        config.thread_count,
        config.node_mask,
        config.processor_mask
    );

    if (config.yield_strategy.empty()) params.mYieldStrategy = Theron::YIELD_STRATEGY_STRONG;
    //else if (config.yield_strategy == "BLOCKING") params.mYieldStrategy = Theron::YIELD_STRATEGY_BLOCKING;
    else if (config.yield_strategy == "POLITE") params.mYieldStrategy = Theron::YIELD_STRATEGY_POLITE;
    else if (config.yield_strategy == "STRONG") params.mYieldStrategy = Theron::YIELD_STRATEGY_STRONG;
    else if (config.yield_strategy == "AGGRESSIVE") params.mYieldStrategy = Theron::YIELD_STRATEGY_AGGRESSIVE;
    else throw std::runtime_error("gras::ThreadPoolConfig yield_strategy unknown: " + config.yield_strategy);

    this->reset(new Theron::Framework(Theron::Framework::Parameters(params)));
}

/***********************************************************************
 * Active framework implementation
 **********************************************************************/
static boost::weak_ptr<Theron::Framework> weak_framework;

void ThreadPool::set_active(void)
{
    weak_framework = *this;
}

static ThreadPool active_thread_pool;

static ThreadPool get_active_thread_pool(void)
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

BlockActor::~BlockActor(void)
{
    //NOP
}
