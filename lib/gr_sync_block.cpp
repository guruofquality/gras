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

#include <gr_sync_block.h>
#include <gr_sync_interpolator.h>
#include <gr_sync_decimator.h>

gr_sync_block::gr_sync_block(void)
{
    //NOP
}

gr_sync_block::gr_sync_block(
    const std::string &name,
    gr_io_signature_sptr input_signature,
    gr_io_signature_sptr output_signature
):
    gr_block(name, input_signature, output_signature)
{
    this->set_fixed_rate(true);
}

gr_sync_interpolator::gr_sync_interpolator(void)
{
    //NOP
}

gr_sync_interpolator::gr_sync_interpolator(
    const std::string &name,
    gr_io_signature_sptr input_signature,
    gr_io_signature_sptr output_signature,
    const size_t interp_rate
):
    gr_sync_block(name, input_signature, output_signature)
{
    this->set_interpolation(interp_rate);
}

size_t gr_sync_interpolator::interpolation(void)
{
    return size_t(1.0*this->relative_rate());
}

void gr_sync_interpolator::set_interpolation(const size_t interp)
{
    this->set_relative_rate(1.0*interp);
}

gr_sync_decimator::gr_sync_decimator(void)
{
    //NOP
}

gr_sync_decimator::gr_sync_decimator(
    const std::string &name,
    gr_io_signature_sptr input_signature,
    gr_io_signature_sptr output_signature,
    const size_t decim_rate
):
    gr_sync_block(name, input_signature, output_signature)
{
    this->set_decimation(decim_rate);
}

size_t gr_sync_decimator::decimation(void)
{
    return size_t(1.0/this->relative_rate());
}

void gr_sync_decimator::set_decimation(const size_t decim)
{
    this->set_relative_rate(1.0/decim);
}
