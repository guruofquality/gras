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

#include "element_impl.hpp"
#include <gnuradio/top_block.hpp>
#include <boost/thread/thread.hpp> //sleep

using namespace gnuradio;

GlobalBlockConfig::GlobalBlockConfig(void)
{
    maximum_output_items = 0;
    buffer_affinity = -1;
}

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
    this->executor->post_all(TopInertMessage());
    this->topology->clear_all();;
    this->executor->commit();
    if (ARMAGEDDON) std::cerr
        << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
        << "xx Top Block Destroyed: " << name << "\n"
        << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
        << std::flush;
}

GlobalBlockConfig TopBlock::global_config(void) const
{
    return (*this)->top_config;
}

void TopBlock::set_global_config(const GlobalBlockConfig &config)
{
    (*this)->top_config = config;
}

void TopBlock::commit(void)
{
    this->start(); //ok to re-start, means update
}

void TopBlock::start(void)
{
    (*this)->executor->commit();
    {
        (*this)->executor->post_all((*this)->thread_group);
    }
    {
        TopTokenMessage message;
        message.token = (*this)->token;
        (*this)->executor->post_all(message);
    }
    {
        //send the global block config before alloc
        (*this)->executor->post_all((*this)->top_config);
    }
    {
        (*this)->executor->post_all(TopAllocMessage());
    }
    {
        (*this)->executor->post_all(TopActiveMessage());
    }
}

void TopBlock::stop(void)
{
    //interrupt these "special" threads
    (*this)->thread_group->interrupt_all();

    //message all blocks to mark done
    (*this)->executor->post_all(TopInertMessage());
}

void TopBlock::run(void)
{
    this->start();
    this->wait();
    this->disconnect_all();
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
