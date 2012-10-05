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

#ifndef INCLUDED_GNURADIO_GR_TOP_BLOCK_H
#define INCLUDED_GNURADIO_GR_TOP_BLOCK_H

#include <gnuradio/top_block.hpp>
#include <gnuradio/gr_hier_block2.h>

struct GRAS_API gr_top_block : gnuradio::TopBlock
{

    gr_top_block(void);

    gr_top_block(const std::string &name);

    void lock(void)
    {
        //NOP
    }

    void unlock(void)
    {
        this->commit();
    }

    void start(void)
    {
        gnuradio::TopBlock::start();
    }

    void start(const size_t max_items)
    {
        this->set_max_noutput_items(max_items);
        gnuradio::TopBlock::start();
    }

    void run(void)
    {
        gnuradio::TopBlock::run();
    }

    void run(const size_t max_items)
    {
        this->set_max_noutput_items(max_items);
        gnuradio::TopBlock::run();
    }

    int max_noutput_items(void) const
    {
        return this->global_config().maximum_output_items;
    }

    void set_max_noutput_items(int max_items)
    {
        gnuradio::GlobalBlockConfig config = this->global_config();
        config.maximum_output_items = max_items;
        this->set_global_config(config);
    }

};

typedef boost::shared_ptr<gr_top_block> gr_top_block_sptr;

GRAS_API gr_top_block_sptr gr_make_top_block(const std::string &name);

#endif /*INCLUDED_GNURADIO_GR_TOP_BLOCK_H*/
