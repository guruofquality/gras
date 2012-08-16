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

#ifndef INCLUDED_GNURADIO_GR_SYNC_BLOCK_H
#define INCLUDED_GNURADIO_GR_SYNC_BLOCK_H

#include <gnuradio/runtime_api.h>
#include <gr_block.h>

struct GR_RUNTIME_API gr_sync_block : gr_block
{

    gr_sync_block(void);

    gr_sync_block(
        const std::string &name,
        gr_io_signature_sptr input_signature,
        gr_io_signature_sptr output_signature
    );

    //! implements work -> calls work
    inline int general_work(
        int noutput_items,
        gr_vector_int &ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    ){
        return this->work(noutput_items, input_items, output_items);
    }

    void set_alignment(const size_t alignment);
    bool is_unaligned(void);

   /*!
    * \brief just like gr_block::general_work, only this arranges to call consume_each for you
    *
    * The user must override work to define the signal processing code
    */
    virtual int work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    ) = 0;

};

#endif /*INCLUDED_GNURADIO_GR_SYNC_BLOCK_H*/
