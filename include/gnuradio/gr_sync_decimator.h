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

#ifndef INCLUDED_GNURADIO_GR_SYNC_DECIMATOR_H
#define INCLUDED_GNURADIO_GR_SYNC_DECIMATOR_H

#include <gnuradio/runtime_api.h>
#include <gr_sync_block.h>

struct GR_RUNTIME_API gr_sync_decimator : gr_sync_block
{

    gr_sync_decimator(void);

    gr_sync_decimator(
        const std::string &name,
        gr_io_signature_sptr input_signature,
        gr_io_signature_sptr output_signature,
        const size_t decim_rate
    );

};

#endif /*INCLUDED_GNURADIO_GR_SYNC_DECIMATOR_H*/
