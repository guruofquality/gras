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
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef INCLUDED_GNURADIO_GR_BLOCK_H
#define INCLUDED_GNURADIO_GR_BLOCK_H

#include <gnuradio/block.hpp>
#include <gnuradio/gr_io_signature.h>
#include <gnuradio/gr_types.h>
#include <gnuradio/gr_tags.h>
#include <string>

typedef std::vector<int> gr_vector_int;
typedef std::vector<void *> gr_vector_void_star;
typedef std::vector<const void *> gr_vector_const_void_star;

namespace gnuradio
{
//! dummy entry, just here for legacy purposes
template <typename T>
boost::shared_ptr<T> get_initial_sptr(T *p)
{
    return boost::shared_ptr<T>(p);
}
}

struct GRAS_API gr_block : gnuradio::Block
{

    gr_block(void);

    gr_block(
        const std::string &name,
        gr_io_signature_sptr input_signature,
        gr_io_signature_sptr output_signature
    );

    template <typename T> void set_msg_handler(T msg_handler){/*LOL*/}

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
    virtual int general_work(
        int noutput_items,
        gr_vector_int &ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    );

    unsigned history(void) const
    {
        //implement off-by-one history compat
        return this->input_config().lookahead_items+1;
    }

    void set_history(unsigned history)
    {
        gnuradio::InputPortConfig config = this->input_config();
        //implement off-by-one history compat
        if (history == 0) history++;
        config.lookahead_items = history-1;
        this->set_input_config(config);
    }

    unsigned output_multiple(void) const
    {
        return this->output_config().reserve_items+1;
    }

    void set_output_multiple(unsigned multiple)
    {
        gnuradio::OutputPortConfig config = this->output_config();
        config.reserve_items = multiple;
        this->set_output_config(config);
    }

    void set_alignment(const size_t alignment);

    bool is_unaligned(void);

    size_t fixed_rate_noutput_to_ninput(const size_t noutput_items);

    size_t interpolation(void) const;

    void set_interpolation(const size_t);

    size_t decimation(void) const;

    void set_decimation(const size_t);

    ///////////// TODO //////////////////////
    int max_noutput_items(){return 0;}
    void set_max_noutput_items(int){}
    void unset_max_noutput_items(){}
    bool is_set_max_noutput_items(){return false;}
    void set_max_output_buffer(long){}
    void set_max_output_buffer(int, long){}
    long max_output_buffer(size_t){return 0;}
    void set_min_output_buffer(long){}
    void set_min_output_buffer(int, long){}
    long min_output_buffer(size_t){return 0;}

};

typedef boost::shared_ptr<gr_block> gr_block_sptr;

#endif /*INCLUDED_GNURADIO_GR_BLOCK_H*/
