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

#ifndef INCLUDED_GNURADIO_TOP_BLOCK_HPP
#define INCLUDED_GNURADIO_TOP_BLOCK_HPP

#include <gnuradio/runtime_api.h>
#include <gnuradio/hier_block.hpp>

namespace gnuradio
{

struct GR_RUNTIME_API TopBlock : HierBlock
{
    TopBlock(void);

    TopBlock(const std::string &name);

    void update(void);

    void run(void)
    {
        this->start();
        this->stop();
        this->wait();
    }

    void start(void);
    void stop(void);
    void wait(void);
};

} //namespace gnuradio

#endif /*INCLUDED_GNURADIO_TOP_BLOCK_HPP*/
