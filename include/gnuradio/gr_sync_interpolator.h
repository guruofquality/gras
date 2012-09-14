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

#ifndef INCLUDED_GNURADIO_GR_SYNC_INTERPOLATOR_H
#define INCLUDED_GNURADIO_GR_SYNC_INTERPOLATOR_H

#include <gnuradio/gr_sync_block.h>

struct GRAS_API gr_sync_interpolator : gr_sync_block
{

    gr_sync_interpolator(void);

    gr_sync_interpolator(
        const std::string &name,
        gr_io_signature_sptr input_signature,
        gr_io_signature_sptr output_signature,
        const size_t interp_rate
    );

};

#endif /*INCLUDED_GNURADIO_GR_SYNC_INTERPOLATOR_H*/
