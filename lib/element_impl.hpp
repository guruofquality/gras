// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_LIBGRAS_ELEMENT_IMPL_HPP
#define INCLUDED_LIBGRAS_ELEMENT_IMPL_HPP

#include <gras_impl/block_actor.hpp>
#include <Apology/Topology.hpp>
#include <Apology/Executor.hpp>
#include <gras/element.hpp>
#include <gras/block.hpp>
#include <gras_impl/token.hpp>
#include <gras_impl/interruptible_thread.hpp>
#include <boost/foreach.hpp>
#include <map>

namespace gras
{

struct ElementImpl
{

    //deconstructor stuff
    ~ElementImpl(void);
    void top_block_cleanup(void);
    void hier_block_cleanup(void);
    void block_cleanup(void);

    //element identification
    std::string name;
    std::string repr;
    boost::shared_ptr<std::string> uid;

    boost::shared_ptr<WeakContainer> weak_self;

    //top block stuff
    SharedThreadGroup thread_group;
    Token token;
    GlobalBlockConfig top_config;

    //element tree stuff
    Element parent;
    std::map<std::string, Element> children;

    //things may be in this element
    boost::shared_ptr<Apology::Topology> topology;
    boost::shared_ptr<Apology::Executor> executor;
    boost::shared_ptr<BlockActor> block;
    boost::shared_ptr<BlockData> block_data;
    ThreadPool thread_pool;
    Apology::Base *get_elem(void) const
    {
        if (block) return block.get();
        return topology.get();
    }

    template <typename MessageType>
    void bcast_prio_msg(const MessageType &msg)
    {
        Theron::Receiver receiver;
        BOOST_FOREACH(Apology::Worker *worker, this->executor->get_workers())
        {
            MessageType message = msg;
            message.prio_token = dynamic_cast<BlockActor *>(worker)->prio_token;
            worker->GetFramework().Send(message, receiver.GetAddress(), worker->GetAddress());
        }
        size_t outstandingCount(this->executor->get_workers().size());
        while (outstandingCount != 0)
        {
            outstandingCount -= receiver.Wait(outstandingCount);
        }
    }

};

} //namespace gras

#endif /*INCLUDED_LIBGRAS_ELEMENT_IMPL_HPP*/
