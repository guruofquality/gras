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

#ifndef INCLUDED_GNURADIO_GR_BLOCK_H
#define INCLUDED_GNURADIO_GR_BLOCK_H

#include <gnuradio/runtime_api.h>
#include <gnuradio/block.hpp>
#include <gr_io_signature.h>
#include <gr_sptr_magic.h>
#include <gr_types.h>
#include <gr_tags.h>
#include <string>

struct GR_RUNTIME_API gr_block : gnuradio::Block
{

    gr_block(void);

    gr_block(
        const std::string &name,
        gr_io_signature_sptr input_signature,
        gr_io_signature_sptr output_signature
    );

    template <typename T> void set_msg_handler(T msg_handler){/*LOL*/}

    void set_output_signature(gr_io_signature_sptr);

    void set_input_signature(gr_io_signature_sptr);

    gr_io_signature_sptr input_signature(void);

    gr_io_signature_sptr output_signature(void);

    void set_fixed_rate(bool fixed_rate);

    //! implements work -> calls general work
    int work(
        const InputItems &input_items,
        const OutputItems &output_items
    );

    /*!
    * \brief compute output items from input items
    *
    * \param noutput_items	number of output items to write on each output stream
    * \param ninput_items	number of input items available on each input stream
    * \param input_items		vector of pointers to the input items, one entry per input stream
    * \param output_items	vector of pointers to the output items, one entry per output stream
    *
    * \returns number of items actually written to each output stream, or -1 on EOF.
    * It is OK to return a value less than noutput_items.  -1 <= return value <= noutput_items
    *
    * general_work must call consume or consume_each to indicate how many items
    * were consumed on each input stream.
    */
    virtual int general_work (int noutput_items,
                gr_vector_int &ninput_items,
                gr_vector_const_void_star &input_items,
                gr_vector_void_star &output_items) = 0;
};

#endif /*INCLUDED_GNURADIO_GR_BLOCK_H*/
