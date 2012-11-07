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
#include <gras/hier_block.hpp>

using namespace gras;

HierBlock::HierBlock(void)
{
    //NOP
}

HierBlock::HierBlock(const std::string &name):
    Element(name)
{
    (*this)->topology = boost::shared_ptr<Apology::Topology>(new Apology::Topology());
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
    boost::shared_ptr<Element> shared_self = elem.weak_self.lock();
    if (shared_self) return shared_self;
    return elem;
}

void HierBlock::connect(
    const Element &src,
    const size_t src_index,
    const Element &sink,
    const size_t sink_index
){
    //TODO, this is the perfect place to validate IO sigs
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
