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
#include <gnuradio/hier_block.hpp>

using namespace gnuradio;

HierBlock::HierBlock(void)
{
    //NOP
}

HierBlock::HierBlock(const std::string &name):
    Element(name)
{
    tsbe::TopologyConfig config;
    (*this)->topology = tsbe::Topology(config);
}

void HierBlock::connect(const Element &elem)
{
    (*this)->topology.add_topology(elem->topology);
}

void HierBlock::disconnect(const Element &elem)
{
    (*this)->topology.remove_topology(elem->topology);
}

void HierBlock::connect(
    const Element &src,
    const size_t src_index,
    const Element &sink,
    const size_t sink_index
){
    const tsbe::Connection conn(
        tsbe::Port(src->get_elem(), src_index),
        tsbe::Port(sink->get_elem(), sink_index)
    );
    (*this)->topology.connect(conn);
}

void HierBlock::disconnect(
    const Element &src,
    const size_t src_index,
    const Element &sink,
    const size_t sink_index
){
    const tsbe::Connection conn(
        tsbe::Port(src->get_elem(), src_index),
        tsbe::Port(sink->get_elem(), sink_index)
    );
    (*this)->topology.disconnect(conn);
}
