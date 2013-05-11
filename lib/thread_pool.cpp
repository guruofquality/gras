// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/thread_pool.hpp>
#include <boost/thread.hpp> //mutex, thread, hardware_concurrency
#include <Theron/EndPoint.h>
#include <Theron/Framework.h>
#include <Theron/Detail/Threading/Utils.h> //prio test
#include <stdexcept>
#include <iostream>

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

static void test_thread_priority_thread(
    const float thread_priority,
    bool &result, bool &called,
    boost::mutex &mutex
)
{
    std::cout << "Created thread to test priority " << thread_priority << std::endl;
    result = Theron::Detail::Utils::SetThreadRelativePriority(thread_priority);
    called = true;
    mutex.unlock();
}

bool ThreadPool::test_thread_priority(const float thread_priority)
{
    boost::mutex mutex;
    boost::thread_group thread_group;
    bool result = false;
    bool called = false;
    mutex.lock();
    thread_group.create_thread(boost::bind(
        &test_thread_priority_thread,
        thread_priority,
        boost::ref(result),
        boost::ref(called),
        boost::ref(mutex)
    ));
    mutex.lock();
    mutex.unlock();
    thread_group.join_all();
    if (not called) throw std::runtime_error("test_thread_priority_thread never executed");
    return result;
}
