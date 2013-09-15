// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "element_impl.hpp"
#include <gras/block.hpp>
#include <boost/thread/thread.hpp> //sleep
#include <iostream>

using namespace gras;

Block::Block(void)
{
    //NOP
}

Block::Block(const std::string &name):
    Element(name)
{
    //create non-actor containers
    (*this)->block_data.reset(new BlockData());
    (*this)->block_data->block = this;
    (*this)->worker.reset(new Apology::Worker());

    //create actor and init members
    (*this)->block_actor.reset(BlockActor::make());
    (*this)->setup_actor();

    //setup some state variables
    (*this)->block_data->block_state = BLOCK_STATE_INIT;
}

Block::~Block(void)
{
    //NOP
}

void ElementImpl::setup_actor(void)
{
    this->block_actor->worker = this->worker.get();
    this->block_actor->name = name; //for debug purposes
    this->block_actor->data = this->block_data;
    this->worker->set_actor(this->block_actor.get());
    this->thread_pool = this->block_actor->thread_pool; //ref copy of pool
}

enum block_cleanup_state_type
{
    BLOCK_CLEANUP_WAIT,
    BLOCK_CLEANUP_WARN,
    BLOCK_CLEANUP_DAMN,
    BLOCK_CLEANUP_DOTS,
};

static void wait_actor_idle(const std::string &repr, Theron::Actor &actor)
{
    const boost::system_time start = boost::get_system_time();
    block_cleanup_state_type state = BLOCK_CLEANUP_WAIT;
    while (actor.GetNumQueuedMessages())
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
        switch (state)
        {
        case BLOCK_CLEANUP_WAIT:
            if (boost::get_system_time() > start + boost::posix_time::seconds(1))
            {
                std::cerr << repr << ", waiting for you to finish." << std::endl;
                state = BLOCK_CLEANUP_WARN;
            }
            break;

        case BLOCK_CLEANUP_WARN:
            if (boost::get_system_time() > start + boost::posix_time::seconds(2))
            {
                std::cerr << repr << ", give up the thread context!" << std::endl;
                state = BLOCK_CLEANUP_DAMN;
            }
            break;

        case BLOCK_CLEANUP_DAMN:
            if (boost::get_system_time() > start + boost::posix_time::seconds(3))
            {
                std::cerr << repr << " FAIL; application will now hang..." << std::endl;
                state = BLOCK_CLEANUP_DOTS;
            }
            break;

        case BLOCK_CLEANUP_DOTS: break;
        }
    }
}

void ElementImpl::block_cleanup(void)
{
    //wait for actor to chew through enqueued messages
    wait_actor_idle(this->repr, *this->block_actor);

    //delete the actor
    this->block_actor.reset();

    //unref actor's framework
    this->thread_pool.reset(); //must be deleted after actor
}

template <typename V>
const typename V::value_type &vector_get_const(const V &v, const size_t index)
{
    if (v.size() <= index)
    {
        return v.back();
    }
    return v[index];
}

template <typename V>
typename V::value_type &vector_get_resize(V &v, const size_t index)
{
    if (v.size() <= index)
    {
        if (v.empty()) v.resize(1);
        v.resize(index+1, v.back());
    }
    return v[index];
}

InputPortConfig &Block::input_config(const size_t which_input)
{
    return vector_get_resize((*this)->block_data->input_configs, which_input);
}

const InputPortConfig &Block::input_config(const size_t which_input) const
{
    return vector_get_const((*this)->block_data->input_configs, which_input);
}

OutputPortConfig &Block::output_config(const size_t which_output)
{
    return vector_get_resize((*this)->block_data->output_configs, which_output);
}

const OutputPortConfig &Block::output_config(const size_t which_output) const
{
    return vector_get_const((*this)->block_data->output_configs, which_output);
}

void Block::commit_config(void)
{
    Theron::Actor &actor = *((*this)->block_actor);

    //handle thread pool migration
    const ThreadPool &thread_pool = this->global_config().thread_pool;
    if (thread_pool and thread_pool != (*this)->block_actor->thread_pool)
    {
        boost::shared_ptr<BlockActor> old_actor = (*this)->block_actor;
        (*this)->block_actor.reset(BlockActor::make(thread_pool));
        (*this)->setup_actor();
        wait_actor_idle((*this)->repr, *old_actor);
    }

    //update messages for in and out ports
    for (size_t i = 0; i < (*this)->worker->get_num_inputs(); i++)
    {
        InputUpdateMessage message;
        message.index = i;
        actor.GetFramework().Send(message, Theron::Address::Null(), actor.GetAddress());
    }
    for (size_t i = 0; i < (*this)->worker->get_num_outputs(); i++)
    {
        OutputUpdateMessage message;
        message.index = i;
        actor.GetFramework().Send(message, Theron::Address::Null(), actor.GetAddress());
    }
}

void Block::notify_active(void)
{
    //NOP
}

void Block::notify_inactive(void)
{
    //NOP
}

void Block::notify_topology(const size_t, const size_t)
{
    return;
}
