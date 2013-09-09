// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "element_impl.hpp"
#include <gras/top_block.hpp>
#include <boost/thread/thread.hpp> //sleep

using namespace gras;

TopBlock::TopBlock(void)
{
    //NOP
}

TopBlock::TopBlock(const std::string &name):
    HierBlock(name)
{
    (*this)->executor.reset(new Apology::Executor((*this)->topology.get()));
    (*this)->token = Token::make();
    (*this)->thread_group = SharedThreadGroup(new boost::thread_group());
}

TopBlock::~TopBlock(void)
{
    //NOP
}

void ElementImpl::top_block_cleanup(void)
{
    this->bcast_prio_msg(TopInertMessage());
    this->topology->clear_all();
    this->executor->commit();
}

void TopBlock::commit(void)
{
    this->start(); //ok to re-start, means update
}

void TopBlock::start(void)
{
    (*this)->executor->commit();
    {
        TopThreadMessage message;
        message.thread_group = (*this)->thread_group;
        (*this)->bcast_prio_msg(message);
    }
    {
        TopTokenMessage message;
        message.token = (*this)->token;
        (*this)->bcast_prio_msg(message);
    }
    {
        //send the global block config before alloc
        TopConfigMessage message;
        message.config = (*this)->global_config;
        (*this)->bcast_prio_msg(message);
    }
    {
        (*this)->bcast_prio_msg(TopAllocMessage());
    }
    {
        (*this)->bcast_prio_msg(TopActiveMessage());
    }
}

void TopBlock::stop(void)
{
    //interrupt these "special" threads
    (*this)->thread_group->interrupt_all();

    //message all blocks to mark done
    (*this)->bcast_prio_msg(TopInertMessage());
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

static const boost::posix_time::time_duration CHECK_DONE_INTERVAL = boost::posix_time::milliseconds(100);
static const boost::posix_time::time_duration INPUT_DONE_GRACE_PERIOD = boost::posix_time::milliseconds(100);

void TopBlock::wait(void)
{
    //We do not need to join "special" threads;
    //the token mechainism will do just fine.
    //FIXME it might be nice to uncomment this for some nice quiet wait(),
    //however, thread group cant be joined twice and this breaks some qa code
    //(*this)->thread_group->join_all();

    //QA lockup detection setup
    bool lockup_debug = getenv("GRAS_LOCKUP_DEBUG") != NULL;
    boost::system_time check_done_time = boost::get_system_time();
    bool has_a_done = false;

    //wait for all blocks to release the token
    while (not (*this)->token.unique())
    {
        wait_thread_yield();

        //determine if we should check on the done status
        if (boost::get_system_time() < check_done_time) continue;
        check_done_time += CHECK_DONE_INTERVAL;

        //optional dot print to understand lockup condition
        if (has_a_done and lockup_debug)
        {
            std::cerr << TopBlock::query("{\"path\":\"/topology.dot\"}") << std::endl;
            lockup_debug = false;
        }

        //loop through blocks looking for non-done blocks with done inputs
        BOOST_FOREACH(Apology::Worker *w, (*this)->topology->get_workers())
        {
            BlockActor *actor = dynamic_cast<BlockActor *>(w->get_actor());
            if (actor->data->block_state == BLOCK_STATE_DONE) has_a_done = true;
            if (actor->data->inputs_done.size() and actor->data->inputs_done.any())
            {
                const boost::system_time grace_over = actor->data->first_input_done_time + INPUT_DONE_GRACE_PERIOD;
                if ((not lockup_debug) and (boost::get_system_time() > grace_over))
                {
                    actor->GetFramework().Send(TopInertMessage(), Theron::Address::Null(), actor->GetAddress());
                }
            }
        }
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
