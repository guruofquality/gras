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
#include <gnuradio/element.hpp>
#include <gnuradio/block.hpp>
#include <gr_types.h>
#include <vector>

namespace gnuradio
{

struct ElementImpl
{
    //stuff for when its a block
    std::string name;
    long unique_id;

    std::vector<size_t> input_items_sizes;
    std::vector<size_t> output_items_sizes;
    std::vector<size_t> input_history_items;
    std::vector<size_t> output_multiple_items;

    //work buffers
    gr_vector_const_void_star work_input_items;
    gr_vector_void_star work_output_items;
    gr_vector_int work_ninput_items;
    Block::InputItems input_items;
    Block::OutputItems output_items;

    tsbe::Block block;

    double relative_rate;
};

} //namespace gnuradio

#endif /*INCLUDED_LIBGNURADIO_ELEMENT_IMPL_HPP*/
