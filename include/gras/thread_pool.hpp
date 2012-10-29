//
// Copyright 2012 Josh Blum
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef INCLUDED_GRAS_THREAD_POOL_HPP
#define INCLUDED_GRAS_THREAD_POOL_HPP

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
     * POLITE,              ///< Threads go to sleep when not in use.
     * STRONG,              ///< Threads yield to other threads but don't go to sleep.
     * AGGRESSIVE           ///< Threads never yield to other threads.
     * Default is STRONG.
     */
    std::string yield_strategy;
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
};

} //namespace gras

#endif /*INCLUDED_GRAS_THREAD_POOL_HPP*/
