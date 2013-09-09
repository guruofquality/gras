// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "element_impl.hpp"
#include <gras/hier_block.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <exception>

using namespace gras;

HierBlock::HierBlock(void)
{
    //NOP
}

HierBlock::HierBlock(const std::string &name):
    Element(name)
{
    (*this)->topology.reset(new Apology::Topology());
}

HierBlock::~HierBlock(void)
{
    //NOP
}

void HierBlock::commit_config(void)
{
    BOOST_FOREACH(Apology::Worker *w, (*this)->topology->get_workers())
    {
        BlockActor *actor = dynamic_cast<BlockActor *>(w->get_actor());
        actor->data->block->global_config().merge((*this)->global_config);
    }
}

void ElementImpl::hier_block_cleanup(void)
{
    this->topology->clear_all();
}

void HierBlock::connect(const Element &elem)
{
    (*this)->topology->add_topology(elem->topology.get());
}

void HierBlock::disconnect(const Element &elem)
{
    (*this)->topology->remove_topology(elem->topology.get());
}

static Apology::Wax get_ref(const Element &elem)
{
    boost::shared_ptr<WeakContainer> weak_self = elem->weak_self;
    if (weak_self)
    {
        boost::shared_ptr<const void> shared_self = weak_self->lock();
        if (shared_self) return shared_self;
    }
    return elem;
}

void HierBlock::connect(
    const Element &src,
    const size_t src_index,
    const Element &sink,
    const size_t sink_index
){
    const Apology::Flow flow(
        Apology::Port(src->get_elem(), src_index, get_ref(src)),
        Apology::Port(sink->get_elem(), sink_index, get_ref(sink))
    );
    (*this)->topology->add_flow(flow);
}

void HierBlock::disconnect(
    const Element &src,
    const size_t src_index,
    const Element &sink,
    const size_t sink_index
){
    const Apology::Flow flow(
        Apology::Port(src->get_elem(), src_index),
        Apology::Port(sink->get_elem(), sink_index)
    );
    (*this)->topology->remove_flow(flow);
}

void HierBlock::disconnect_all(void)
{
    (*this)->topology->clear_all();
}

void HierBlock::commit(void)
{
    (*this)->topology->commit();
}
