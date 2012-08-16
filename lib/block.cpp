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
#include <gnuradio/block.hpp>
#include <boost/detail/atomic_count.hpp>

using namespace gnuradio;

static boost::detail::atomic_count unique_id_pool(0);

Block::Block(void)
{
    //NOP
}

Block::Block(const std::string &name)
{
    this->reset(new ElementImpl());
    (*this)->name = name;
    (*this)->unique_id = ++unique_id_pool; //not really thread safe but ok
    (*this)->input_history_items.push_back(0);
    (*this)->output_multiple_items.push_back(1);
    this->set_relative_rate(1.0);
}

long Block::unique_id(void) const
{
    return (*this)->unique_id;
}

std::string Block::name(void) const
{
    return (*this)->name;
}

size_t Block::history(const size_t which_input) const
{
    if ((*this)->input_history_items.size() <= which_input)
    {
        return (*this)->input_history_items[0];
    }
    return (*this)->input_history_items[which_input];
}

void Block::set_history(const size_t history, const size_t which_input)
{
    if ((*this)->input_history_items.size() <= which_input)
    {
        (*this)->input_history_items.resize(which_input+1, history);
    }
    (*this)->input_history_items[which_input] = history;
}

size_t Block::output_multiple(const size_t which_input) const
{
    if ((*this)->output_multiple_items.size() <= which_input)
    {
        return (*this)->output_multiple_items[0];
    }
    return (*this)->output_multiple_items[which_input];
}

void Block::set_output_multiple(const size_t multiple, const size_t which_input)
{
    if ((*this)->output_multiple_items.size() <= which_input)
    {
        (*this)->output_multiple_items.resize(which_input+1, multiple);
    }
    (*this)->output_multiple_items[which_input] = multiple;
}

void Block::set_relative_rate(double relative_rate)
{
    (*this)->relative_rate = relative_rate;
}

double Block::relative_rate(void) const
{
    return (*this)->relative_rate;
}
