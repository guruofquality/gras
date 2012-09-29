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

#include <gras_impl/block_actor.hpp>
#include <boost/thread/thread.hpp>
#include <Theron/Framework.h>

using namespace gnuradio;

static size_t hardware_concurrency(void)
{
    const size_t n = boost::thread::hardware_concurrency();
    return std::max(size_t(2), n);
}

static Theron::Framework &get_global_framework(void)
{
    static Theron::Framework framework(hardware_concurrency());
    return framework;
}

BlockActor::BlockActor(void):
    Apology::Worker(get_global_framework())
{
    this->register_handlers();
}

BlockActor::~BlockActor(void)
{
    //NOP
}
