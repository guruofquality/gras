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

namespace gras
{

struct ElementImpl
{

    //deconstructor stuff
    ~ElementImpl(void);
    void top_block_cleanup(void);
    void hier_block_cleanup(void);
    void block_cleanup(void);

    //common element properties
    std::string name;
    long unique_id;
    std::string id;

    //top block stuff
    SharedThreadGroup thread_group;
    Token token;
    Token active_token;
    GlobalBlockConfig top_config;

    //things may be in this element
    boost::shared_ptr<Apology::Topology> topology;
    boost::shared_ptr<Apology::Executor> executor;
    boost::shared_ptr<BlockActor> block;
    ThreadPool thread_pool;
    Apology::Base *get_elem(void) const
    {
        if (block) return block.get();
        return topology.get();
    }


};

} //namespace gras

#endif /*INCLUDED_LIBGRAS_ELEMENT_IMPL_HPP*/
