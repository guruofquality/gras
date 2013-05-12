// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_THREAD_POOL_HPP
#define INCLUDED_GRAS_THREAD_POOL_HPP

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning (disable:4251)  // needs to have dll interface
#endif //_MSC_VER

#include <gras/gras.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <string>

//! ThreadPool is an unexposed Theron Framework
//! Forward declare the Framwork for c++ users
namespace Theron
{
    class Framework;
}

namespace gras
{

struct GRAS_API ThreadPoolConfig
{
    ThreadPoolConfig(void);

    /*!
     * The initial number of worker threads to create within the framework.
     * Default is the number of CPUs on the system.
     */
    size_t thread_count;

    /*!
     * Specifies the NUMA processor nodes upon which the framework may execute.
     * Default is all NUMA nodes on the system.
     */
    size_t node_mask;

    /*!
     * Specifies the subset of the processors in each NUMA processor node upon which the framework may execute.
     * Default is all CPUs per NUMA node.
     */
    size_t processor_mask;

    /*!
     * Yield strategy employed by the worker threads in the framework.
     * BLOCKING,            ///< Threads block on condition variables.
     * POLITE,              ///< Threads go to sleep when not in use.
     * STRONG,              ///< Threads yield to other threads but don't go to sleep.
     * AGGRESSIVE           ///< Threads never yield to other threads.
     * Default is BLOCKING.
     * The default can be overridden with the GRAS_YIELD environment variable.
     */
    std::string yield_strategy;

    /*!
     * Relative scheduling priority of the worker threads (range -1.0f to 1.0f, 0.0f is "normal").
     * Default is 0.0f
     */
    float thread_priority;
};

/*!
 * Thread Pool is is a this wrapper of Theron Framework, see link for more details:
 * http://docs.theron-library.com/5.00/structTheron_1_1Framework_1_1Parameters.html
 */
struct GRAS_API ThreadPool : boost::shared_ptr<Theron::Framework>
{
    //! Create an empty thread pool
    ThreadPool(void);

    //! Create a thread pool from a weak pointer to a framework
    ThreadPool(boost::weak_ptr<Theron::Framework> p);

    //! Create a new thread pool with parameters
    ThreadPool(const ThreadPoolConfig &config);

    /*!
     * When a block is created, it will execute in the active pool.
     * Use this call before creating a block to control which
     * thread pool that the block's work routine will run in.
     */
    void set_active(void);

    /*!
     * Test that a particular thread priority setting is possible.
     *
     * Highly OS dependent! Some OS require special permissions,
     * or security settings to use real time priority setting.
     *
     * \param thread_priority range -1.0f to 1.0f, 0.0f is "normal"
     * \return true if the priority change is possible
     */
    static bool test_thread_priority(const float thread_priority);
};

} //namespace gras

#ifdef _MSC_VER
#pragma warning(pop)
#endif //_MSC_VER

#endif /*INCLUDED_GRAS_THREAD_POOL_HPP*/
