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
#include <boost/foreach.hpp>

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
    if (this->fixed_rate())
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

//TODO Tag2gr_tag and gr_tag2Tag need PMC to/from PMT logic
//currently PMC holds the pmt_t, this is temporary
static gr_tag_t Tag2gr_tag(const gnuradio::Tag &tag)
{
    gr_tag_t t;
    t.offset = tag.offset;
    t.key = tag.key? tag.key.cast<pmt::pmt_t>() : pmt::pmt_t();
    t.value = tag.value? tag.value.cast<pmt::pmt_t>() : pmt::pmt_t();
    t.srcid = tag.srcid? tag.srcid.cast<pmt::pmt_t>() : pmt::pmt_t();
    return t;
}

static gnuradio::Tag gr_tag2Tag(const gr_tag_t &tag)
{
    return gnuradio::Tag(tag.offset, PMC::make(tag.key), PMC::make(tag.value), PMC::make(tag.srcid));
}

void gr_block::add_item_tag(
    const size_t which_output, const gr_tag_t &tag
){
    this->post_output_tag(which_output, gr_tag2Tag(tag));
}

void gr_block::add_item_tag(
    const size_t which_output,
    uint64_t abs_offset,
    const pmt::pmt_t &key,
    const pmt::pmt_t &value,
    const pmt::pmt_t &srcid
){
    gr_tag_t t;
    t.offset = abs_offset;
    t.key = key;
    t.value = value;
    t.srcid = srcid;
    this->add_item_tag(which_output, t);
}

void gr_block::get_tags_in_range(
    std::vector<gr_tag_t> &tags,
    const size_t which_input,
    uint64_t abs_start,
    uint64_t abs_end,
    const pmt::pmt_t &key
){
    tags.clear();
    BOOST_FOREACH(const gnuradio::Tag &tag, this->get_input_tags(which_input))
    {
        if (tag.offset >= abs_start and tag.offset <= abs_end)
        {
            gr_tag_t t = Tag2gr_tag(tag);
            if (key or pmt::pmt_equal(t.key, key)) tags.push_back(t);
        }
    }
}
