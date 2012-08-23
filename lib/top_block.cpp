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
#include <gnuradio/top_block.hpp>

using namespace gnuradio;

TopBlock::TopBlock(void)
{
    //NOP
}

TopBlock::TopBlock(const std::string &name):
    HierBlock(name)
{
    tsbe::ExecutorConfig config;
    config.topology = (*this)->topology;
    (*this)->executor = tsbe::Executor(config);
}

void TopBlock::update(void)
{
    (*this)->executor.update();
}

void TopBlock::start(void)
{
    (*this)->executor.set_state(STATE_ACTIVE);
}

void TopBlock::stop(void)
{
    (*this)->executor.set_state(STATE_INERT);
}

void TopBlock::wait(void)
{
    //NOP/TODO?
}
