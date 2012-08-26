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
    TopBlockUpdateEvent event;
    event.state = TopBlockUpdateEvent::UPDATE;
    (*this)->executor.update(event);
}

void TopBlock::set_buffer_hint(const size_t hint)
{
    TopBlockUpdateEvent event;
    event.state = TopBlockUpdateEvent::HINT;
    event.hint = hint;
    (*this)->executor.update(event);
}

void TopBlock::start(void)
{
    TopBlockUpdateEvent event;
    event.state = TopBlockUpdateEvent::ACTIVE;
    (*this)->executor.update(event);
}

void TopBlock::stop(void)
{
    TopBlockUpdateEvent event;
    event.state = TopBlockUpdateEvent::INERT;
    (*this)->executor.update(event);
}

void TopBlock::wait(void)
{
    //NOP/TODO?
}
