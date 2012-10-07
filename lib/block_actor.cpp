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

#include <gnuradio/thread_pool.hpp>
#include <gras_impl/block_actor.hpp>
#include <boost/thread/thread.hpp>
#include <Theron/Framework.h>

using namespace gnuradio;

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

const size_t default_concurrency(void)
{
    const size_t n = boost::thread::hardware_concurrency();
    return std::max(size_t(2), n);
}

ThreadPool::ThreadPool(unsigned long threadCount)
{
    if (threadCount == 0) threadCount = default_concurrency();
    this->reset(new Theron::Framework(Theron::Framework::Parameters(threadCount, 0)));
}

ThreadPool::ThreadPool(const unsigned long threadCount, const unsigned long nodeMask)
{
    this->reset(new Theron::Framework(Theron::Framework::Parameters(threadCount, nodeMask)));
}

ThreadPool::ThreadPool(const unsigned long threadCount, const unsigned long nodeMask, const unsigned long processorMask)
{
    this->reset(new Theron::Framework(Theron::Framework::Parameters(threadCount, nodeMask, processorMask)));
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
        active_thread_pool = ThreadPool(0);
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
    thread_pool = get_active_thread_pool();
    active_thread_pool.reset(); //actors hold this, now its safe to reset, weak_framework only
    this->register_handlers();
}

BlockActor::~BlockActor(void)
{
    this->mark_done();
}
