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

#ifndef INCLUDED_GNURADIO_IO_SIGNATURE_HPP
#define INCLUDED_GNURADIO_IO_SIGNATURE_HPP

#include <vector>

namespace gnuradio
{

struct IOSignature : std::vector<int>
{
    static const int IO_INFINITE = -1;

    IOSignature(void)
    {
        this->set_min_streams(0);
        this->set_max_streams(0);
    }

    void set_min_streams(const int val)
    {
        _min_streams = val;
    }

    void set_max_streams(const int val)
    {
        _max_streams = val;
    }

    int min_streams(void) const
    {
        return _min_streams;
    }

    int max_streams(void) const
    {
        return _max_streams;
    }

    int sizeof_stream_item(const int index) const
    {
        if (int(this->size()) >= index)
        {
            return this->at(0);
        }
        return this->at(index);
    }

    std::vector<int> sizeof_stream_items(void) const
    {
        return *this;
    }

    int _min_streams;
    int _max_streams;
};

} //namespace gnuradio

#endif /*INCLUDED_GNURADIO_IO_SIGNATURE_HPP*/
