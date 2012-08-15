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

#ifndef INCLUDED_GNURADIO_HIER_BLOCK_HPP
#define INCLUDED_GNURADIO_HIER_BLOCK_HPP

#include <gnuradio/runtime_api.h>
#include <gnuradio/element.hpp>
#include <gruel/pmt.h>

namespace gnuradio
{

struct GR_RUNTIME_API HierBlock : Element
{
    HierBlock(void);

    void connect(const Element &elem);

    void disconnect(const Element &elem);

    void connect(
        const Element &src,
        const size_t src_index,
        const Element &sink,
        const size_t sink_index
    );

    void disconnect(
        const Element &src,
        const size_t src_index,
        const Element &sink,
        const size_t sink_index
    );

};

} //namespace gnuradio

#endif /*INCLUDED_GNURADIO_HIER_BLOCK_HPP*/
