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

#include "element_impl.hpp"
#include <gnuradio/top_block.hpp>
#include <boost/thread/thread.hpp> //sleep

using namespace gnuradio;

TopBlock::TopBlock(void)
{
    //NOP
}

TopBlock::TopBlock(const std::string &name):
    HierBlock(name)
{
    (*this)->executor = boost::shared_ptr<Apology::Executor>(new Apology::Executor((*this)->topology.get()));
    (*this)->token = Token::make();
    (*this)->thread_group = SharedThreadGroup(new boost::thread_group());
    if (GENESIS) std::cerr
        << "===================================================\n"
        << "== Top Block Created: " << name << "\n"
        << "===================================================\n"
        << std::flush;
}

void ElementImpl::top_block_cleanup(void)
{
    TopBlockMessage event;
    event.what = TopBlockMessage::INERT;
    this->executor->post_all(event);
    if (ARMAGEDDON) std::cerr
        << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
        << "xx Top Block Destroyed: " << name << "\n"
        << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
        << std::flush;
}

void TopBlock::update(void)
{
    this->start(); //ok to re-start, means update
}

void TopBlock::set_buffer_hint(const size_t hint)
{
    TopBlockMessage event;
    event.what = TopBlockMessage::HINT;
    event.hint = hint;
    (*this)->executor->post_all(event);
}

void TopBlock::start(void)
{
    (*this)->executor->commit();
    {
        (*this)->executor->post_all((*this)->thread_group);
    }
    {
        TopBlockMessage event;
        event.what = TopBlockMessage::TOKEN_TIME;
        event.token = (*this)->token;
        (*this)->executor->post_all(event);
    }
    {
        TopBlockMessage event;
        event.what = TopBlockMessage::ALLOCATE;
        (*this)->executor->post_all(event);
    }
    {
        TopBlockMessage event;
        event.what = TopBlockMessage::ACTIVE;
        (*this)->executor->post_all(event);
    }
}

void TopBlock::stop(void)
{
    //interrupt these "special" threads
    (*this)->thread_group->interrupt_all();

    //message all blocks to mark done
    TopBlockMessage event;
    event.what = TopBlockMessage::INERT;
    (*this)->executor->post_all(event);
}

void TopBlock::run(void)
{
    this->start();
    this->wait();
}

GRAS_FORCE_INLINE void wait_thread_yield(void)
{
    //boost::this_thread::yield();
    boost::this_thread::sleep(boost::posix_time::milliseconds(1));
}

void TopBlock::wait(void)
{
    //We do not need to join "special" threads;
    //the token mechainism will do just fine.
    //FIXME it might be nice to uncomment this for some nice quiet wait(),
    //however, thread group cant be joined twice and this breaks some qa code
    //(*this)->thread_group->join_all();

    //wait for all blocks to release the token
    while (not (*this)->token.unique())
    {
        wait_thread_yield();
    }
}

bool TopBlock::wait(const double timeout)
{
    const boost::system_time exit_time = boost::get_system_time() +
        boost::posix_time::microseconds(long(timeout*1e6));

    //wait for all blocks to release the token
    while (not (*this)->token.unique() or boost::get_system_time() < exit_time)
    {
        wait_thread_yield();
    }

    return (*this)->token.unique();
}
