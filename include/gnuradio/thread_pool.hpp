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

#ifndef INCLUDED_GNURADIO_THREAD_POOL_HPP
#define INCLUDED_GNURADIO_THREAD_POOL_HPP

#include <gnuradio/gras.hpp>

//! ThreadPool is an unexposed Theron Framework
//! Forward declare the Framwork for c++ users
namespace Theron
{
    class Framework;
}

namespace gnuradio
{

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
    ThreadPool(const unsigned long threadCount);

    //! Create a new thread pool with parameters
    ThreadPool(const unsigned long threadCount, const unsigned long nodeMask);

    //! Create a new thread pool with parameters
    ThreadPool(const unsigned long threadCount, const unsigned long nodeMask, const unsigned long processorMask);

    /*!
     * When a block is created, it will execute in the active pool.
     * Use this call before creating a block to control which
     * thread pool that the block's work routine will run in.
     */
    void set_active(void);
};

} //namespace gnuradio

#endif /*INCLUDED_GNURADIO_THREAD_POOL_HPP*/
