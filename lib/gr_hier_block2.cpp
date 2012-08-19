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

#include <gr_hier_block2.h>

gr_hier_block2::gr_hier_block2(void)
{
    //NOP
}

gr_hier_block2::gr_hier_block2(
    const std::string &name,
    gr_io_signature_sptr,
    gr_io_signature_sptr
):
    gnuradio::HierBlock(name)
{
    //NOP
}
