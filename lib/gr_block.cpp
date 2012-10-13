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

int gr_block::work(
    const InputItems &input_items,
    const OutputItems &output_items
){
    return this->general_work(
        (*this)->block->work_noutput_items,
        (*this)->block->work_ninput_items,
        (*this)->block->work_input_items,
        (*this)->block->work_output_items
    );
}

void gr_block::forecast(int noutput_items, std::vector<int> &ninputs_req)
{
    for (size_t i = 0; i < ninputs_req.size(); i++)
    {
        ninputs_req[i] = fixed_rate_noutput_to_ninput(noutput_items);
    }
}

int gr_block::general_work(
    int noutput_items,
    gr_vector_int &ninput_items,
    gr_vector_const_void_star &input_items,
    gr_vector_void_star &output_items
){
    throw std::runtime_error("gr_block subclasses must overload general_work!");
}

void gr_block::set_alignment(const size_t)
{
    //TODO
    //probably dont need this since buffers always start aligned
    //and therefore alignment is always re-acheived
}

bool gr_block::is_unaligned(void)
{
    //TODO
    //probably dont need this since volk dispatcher checks alignment
    //32 byte aligned is good enough for you
    return ((*this)->block->work_io_ptr_mask & ptrdiff_t(GRAS_MAX_ALIGNMENT-1)) != 0;
}

size_t gr_block::fixed_rate_noutput_to_ninput(const size_t noutput_items)
{
    if ((*this)->block->enable_fixed_rate)
    {
        return size_t(0.5 + (noutput_items/this->relative_rate())) + this->history() - 1;
    }
    else
    {
        return noutput_items + this->history() - 1;
    }
}

size_t gr_block::interpolation(void) const
{
    return size_t(1.0*this->relative_rate());
}

void gr_block::set_interpolation(const size_t interp)
{
    this->set_relative_rate(1.0*interp);
    this->set_output_multiple(interp);
}

size_t gr_block::decimation(void) const
{
    return size_t(1.0/this->relative_rate());
}

void gr_block::set_decimation(const size_t decim)
{
    this->set_relative_rate(1.0/decim);
}

unsigned gr_block::history(void) const
{
    //implement off-by-one history compat
    return this->input_config().lookahead_items+1;
}

void gr_block::set_history(unsigned history)
{
    gnuradio::InputPortConfig config = this->input_config();
    //implement off-by-one history compat
    if (history == 0) history++;
    config.lookahead_items = history-1;
    this->set_input_config(config);
}

unsigned gr_block::output_multiple(void) const
{
    return (*this)->block->output_multiple_items;
}

void gr_block::set_output_multiple(unsigned multiple)
{
    (*this)->block->output_multiple_items = multiple;
    gnuradio::OutputPortConfig config = this->output_config();
    config.reserve_items = multiple;
    this->set_output_config(config);
}

int gr_block::max_noutput_items(void) const
{
    return this->output_config().maximum_items;
}

void gr_block::set_max_noutput_items(int max_items)
{
    gnuradio::OutputPortConfig config = this->output_config();
    config.maximum_items = max_items;
    this->set_output_config(config);
}

void gr_block::unset_max_noutput_items(void)
{
    this->set_max_noutput_items(0);
}

bool gr_block::is_set_max_noutput_items(void) const
{
    return this->max_noutput_items() != 0;
}
