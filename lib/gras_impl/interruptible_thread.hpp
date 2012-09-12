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
// along with io_sig program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef INCLUDED_LIBGRAS_IMPL_INTERRUPTIBLE_THREAD_HPP
#define INCLUDED_LIBGRAS_IMPL_INTERRUPTIBLE_THREAD_HPP

#include <gras_impl/debug.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

//--------------------------- ATTENTION !!! --------------------------//
//-- The author does not intend to have 2 threading platforms.
//-- This file and its invocations should be removed when source blocks
//-- in the tree can yield the thread context without producing.
//--------------------------------------------------------------------//

/*!
 * This is the only place you will find any threading stuff.
 * The entire point here is that the source's in gnuradio
 * are sometimed bad and block forever (the author is guilty too).
 * This thread pool creates an interruptible thread to perform work.
 * Everything is nice and synchronous with the block actor.
 * The block actor will even block on the work function...
 * However, this will be interrupted and not block forever,
 * when the executor is told to stop/interrupt and wait/join.
 */
namespace gnuradio
{

    typedef boost::shared_ptr<boost::thread_group> SharedThreadGroup;

    struct InterruptibleThread
    {
        typedef boost::function<void(void)> Callable;

        InterruptibleThread(SharedThreadGroup thread_group, Callable callable):
            _thread_group(thread_group),
            _callable(callable)
        {
            _wait_msg = true;
            _wait_ack = true;
            _mutex.lock();
            _thread = _thread_group->create_thread(boost::bind(&InterruptibleThread::run, this));
            _mutex.lock();
            _mutex.unlock();
        }

        ~InterruptibleThread(void)
        {
            {
                boost::mutex::scoped_lock lock(_mutex);
                _callable = Callable();
            }
            _thread->interrupt();
            _thread->join();
        }

        inline void call(void)
        {
            boost::mutex::scoped_lock lock(_mutex);
            if (not _callable) return;
            _wait_msg = false;
            _notify(lock);
            while (_wait_ack) _cond.wait(lock);
            _wait_ack = true;
        }

        void run(void)
        {
            _mutex.unlock(); //spawn barrier unlock
            boost::mutex::scoped_lock lock(_mutex);
            try
            {
                while (not boost::this_thread::interruption_requested())
                {
                    while (_wait_msg) _cond.wait(lock);
                    _wait_msg = true;
                    if (not _callable) break;
                    _callable();
                    _wait_ack = false;
                    _notify(lock);
                }
            }
            catch(const std::exception &ex)
            {
                std::cerr << "InterruptibleThread threw " << ex.what() << std::endl;
            }
            catch(const boost::thread_interrupted &)
            {
                //normal exit is thread_interrupted
            }
            catch(...)
            {
                std::cerr << "InterruptibleThread threw unknown exception" << std::endl;
            }
            _callable = Callable();
            _wait_ack = false;
            _notify(lock);
        }

        template <typename Lock>
        void _notify(Lock &lock)
        {
            lock.unlock();
            _cond.notify_one();
            lock.lock();
        }

        //thread locking mechanisms
        bool _wait_msg;
        bool _wait_ack;
        boost::mutex _mutex;
        boost::condition_variable _cond;
        SharedThreadGroup _thread_group;
        Callable _callable;
        boost::thread *_thread;
    };

} //namespace gnuradio

#endif /*INCLUDED_LIBGRAS_IMPL_INTERRUPTIBLE_THREAD_HPP*/
