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

#define GRAS_API

//not here to fight you swig, reference() is ambigi with shared ptr, but whatevs
%ignore gri_agc_cc::reference();
%ignore gri_agc2_ff::reference();
%ignore gri_agc2_cc::reference();

%{

#include <gnuradio/element.hpp>
#include <gnuradio/block.hpp>
#include <gnuradio/hier_block.hpp>
#include <gnuradio/top_block.hpp>
#include <gnuradio/io_signature.hpp>
#include <gnuradio/tags.hpp>
#include <gr_io_signature.h>
#include <gr_block.h>
#include <gr_hier_block2.h>
#include <gr_message.h>
#include <gr_msg_handler.h>
#include <gr_msg_queue.h>
#include <gr_sync_block.h>
#include <gr_sync_decimator.h>
#include <gr_sync_interpolator.h>

%}

%include <gr_message.i>
%include <gr_msg_handler.i>
%include <gr_msg_queue.i>
%include <gr_swig_block_magic.i>

#ifdef SW_RUNTIME

%include "gras.i"

#else

//the bare minimum block inheritance interface to make things work but keep swig cxx file size down
%include <gnuradio/element.hpp>
namespace gnuradio
{
    struct Block : Element{};
}
struct gr_block : gnuradio::Block{};
struct gr_sync_block : gr_block{};
struct gr_sync_interpolator : gr_sync_block{};
struct gr_sync_decimator : gr_sync_block{};

#endif
