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

#ifndef INCLUDED_GRNURADIO_TYPES_H
#define INCLUDED_GRNURADIO_TYPES_H

// this section is to satisfy swig includes for gnuradio.i
// since gnuradio.i includes gr_types.h, we only have to edit this file
#include <gnuradio/io_signature.hpp>
#include <gnuradio/element.hpp>
#include <gnuradio/block.hpp>
#include <gnuradio/top_block.hpp>
#include <gnuradio/hier_block.hpp>

// and gnuradio apparently needs its own typedefs for stdint...
#ifdef __cplusplus

#include <boost/cstdint.hpp>
typedef boost::int16_t gr_int16;
typedef boost::int32_t gr_int32;
typedef boost::int64_t gr_int64;
typedef boost::uint16_t gr_uint16;
typedef boost::uint32_t gr_uint32;
typedef boost::uint64_t gr_uint64;

#include <complex>
typedef std::complex<float> gr_complex;
typedef std::complex<double> gr_complexd;

#endif

#endif /* INCLUDED_GRNURADIO_TYPES_H */
