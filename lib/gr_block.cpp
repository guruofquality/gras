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

#include "element_impl.hpp"
#include <gr_block.h>

gr_block::gr_block(void)
{
    //NOP
}

gr_block::gr_block(
    const std::string &name,
    gr_io_signature_sptr input_signature,
    gr_io_signature_sptr output_signature
):
    gnuradio::Block(name)
{
    this->set_fixed_rate(false);
    this->set_input_signature(input_signature);
    this->set_output_signature(output_signature);
}

void gr_block::set_output_signature(gr_io_signature_sptr sig)
{
    for (size_t i = 0; i < sig->sizeof_stream_items().size(); i++)
    {
        this->set_output_size(sig->sizeof_stream_items()[i], i);
    }
}

void gr_block::set_input_signature(gr_io_signature_sptr sig)
{
    for (size_t i = 0; i < sig->sizeof_stream_items().size(); i++)
    {
        this->set_input_size(sig->sizeof_stream_items()[i], i);
    }
}

gr_io_signature_sptr gr_block::input_signature(void) const
{
    return (*this)->input_signature;
}

gr_io_signature_sptr gr_block::output_signature(void) const
{
    return (*this)->output_signature;
}

int gr_block::work(
    const InputItems &input_items,
    const OutputItems &output_items
){
    return this->general_work(
        (output_items.empty())? input_items[0].size() : output_items[0].size(),
        (*this)->work_ninput_items,
        (*this)->work_input_items,
        (*this)->work_output_items
    );
}
