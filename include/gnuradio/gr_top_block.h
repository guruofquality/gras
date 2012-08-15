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

#ifndef INCLUDED_GNURADIO_GR_TOP_BLOCK_H
#define INCLUDED_GNURADIO_GR_TOP_BLOCK_H

#include <gnuradio/runtime_api.h>
#include <gr_hier_block2.h>

struct GR_RUNTIME_API gr_top_block : gr_hier_block2
{

    gr_top_block(void);

    gr_top_block(const std::string &name);

};

#endif /*INCLUDED_GNURADIO_GR_TOP_BLOCK_H*/
