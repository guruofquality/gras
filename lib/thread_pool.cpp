// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/thread_pool.hpp>
#include <boost/thread/thread.hpp>
#include <Theron/EndPoint.h>
#include <Theron/Framework.h>
#include <stdexcept>

using namespace gras;

ThreadPoolConfig::ThreadPoolConfig(void)
{
    thread_count = boost::thread::hardware_concurrency();
    thread_count = std::max(size_t(2), thread_count);
    node_mask = 0;
    processor_mask = 0xffffffff;
    yield_strategy = "BLOCKING";
    thread_priority = 0.0f;

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
    else if (config.yield_strategy == "BLOCKING") params.mYieldStrategy = Theron::YIELD_STRATEGY_BLOCKING;
    else if (config.yield_strategy == "POLITE") params.mYieldStrategy = Theron::YIELD_STRATEGY_POLITE;
    else if (config.yield_strategy == "STRONG") params.mYieldStrategy = Theron::YIELD_STRATEGY_STRONG;
    else if (config.yield_strategy == "AGGRESSIVE") params.mYieldStrategy = Theron::YIELD_STRATEGY_AGGRESSIVE;
    else throw std::runtime_error("gras::ThreadPoolConfig yield_strategy unknown: " + config.yield_strategy);

    params.mThreadPriority = config.thread_priority;

    this->reset(new Theron::Framework(Theron::Framework::Parameters(params)));
}
