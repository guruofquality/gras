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

#ifndef INCLUDED_LIBGNURADIO_ELEMENT_IMPL_HPP
#define INCLUDED_LIBGNURADIO_ELEMENT_IMPL_HPP

#include <tsbe/block.hpp>
#include <tsbe/topology.hpp>
#include <gnuradio/element.hpp>
#include <gnuradio/block.hpp>
#include <gr_types.h>
#include <gr_io_signature.h>
#include <vector>

namespace gnuradio
{

struct ElementImpl
{
    //stuff for when its a block
    std::string name;
    long unique_id;

    //per port properties
    std::vector<size_t> input_items_sizes;
    std::vector<size_t> output_items_sizes;
    gr_io_signature_sptr input_signature;
    gr_io_signature_sptr output_signature;
    std::vector<size_t> input_history_items;
    std::vector<size_t> output_multiple_items;

    //keeps track of production
    std::vector<uint64_t> bytes_consumed;
    std::vector<uint64_t> bytes_produced;

    //work buffers
    gr_vector_const_void_star work_input_items;
    gr_vector_void_star work_output_items;
    gr_vector_int work_ninput_items;
    Block::InputItems input_items;
    Block::OutputItems output_items;
    std::vector<size_t> produce_items;
    std::vector<size_t> consume_items;

    //tag tracking
    std::vector<std::vector<Tag> > input_tags;
    std::vector<std::vector<Tag> > output_tags;

    Block::tag_propagation_policy_t tag_prop_policy;

    tsbe::Block block;
    tsbe::Topology topology;
    const tsbe::Element &get_elem(void)
    {
        if (block) return block;
        return topology;
    }

    void handle_port_msg(const size_t, const tsbe::Wax &);
    void topology_update(const tsbe::TaskInterface &);

    bool enble_fixed_rate;
    double relative_rate;
};

} //namespace gnuradio

#endif /*INCLUDED_LIBGNURADIO_ELEMENT_IMPL_HPP*/
