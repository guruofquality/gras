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

#ifndef INCLUDED_GNURADIO_GR_IO_SIGNATURE_H
#define INCLUDED_GNURADIO_GR_IO_SIGNATURE_H

#include <gnuradio/io_signature.hpp>
#include <vector>

typedef gnuradio::IOSignature gr_io_signature;
typedef gr_io_signature gr_io_signature_sptr;

inline gr_io_signature_sptr gr_make_io_signature(
    int min_streams,
    int max_streams,
    int sizeof_stream_item
){
    gnuradio::IOSignature io_sig(min_streams, max_streams);

    if (io_sig.max_streams() != gr_io_signature::IO_INFINITE)
    {
        io_sig.resize(io_sig.max_streams(), sizeof_stream_item);
    }
    else
    {
        io_sig.resize(1, sizeof_stream_item);
    }

    return io_sig;
}

inline gr_io_signature_sptr gr_make_io_signature2(
    int min_streams,
    int max_streams,
    int sizeof_stream_item1,
    int sizeof_stream_item2
){
    gnuradio::IOSignature io_sig(min_streams, max_streams);
    io_sig.push_back(sizeof_stream_item1);
    io_sig.push_back(sizeof_stream_item2);
    return io_sig;
}

inline gr_io_signature_sptr gr_make_io_signature3(
    int min_streams,
    int max_streams,
    int sizeof_stream_item1,
    int sizeof_stream_item2,
    int sizeof_stream_item3
){
    gnuradio::IOSignature io_sig(min_streams, max_streams);
    io_sig.push_back(sizeof_stream_item1);
    io_sig.push_back(sizeof_stream_item2);
    io_sig.push_back(sizeof_stream_item3);
    return io_sig;
}

template <typename T>
inline gr_io_signature_sptr gr_make_io_signaturev(
    int min_streams,
    int max_streams,
    const std::vector<T> &sizeof_stream_items
){
    gnuradio::IOSignature io_sig(min_streams, max_streams);
    for (unsigned i = 0; i < sizeof_stream_items.size(); i++)
    {
        io_sig.push_back(sizeof_stream_items[i]);
    }
    return io_sig;
}

#endif /*INCLUDED_GNURADIO_GR_IO_SIGNATURE_H*/
