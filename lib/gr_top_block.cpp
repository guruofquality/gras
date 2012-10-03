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

#include <gr_top_block.h>

gr_top_block::gr_top_block(void):
    //cannot make a null top block, use name constructor
    gnuradio::TopBlock("top")
{
    //NOP
}

gr_top_block::gr_top_block(const std::string &name):
    gnuradio::TopBlock(name)
{
    //NOP
}

gr_top_block_sptr gr_make_top_block(const std::string &name)
{
    return gr_top_block_sptr(new gr_top_block(name));
}
