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

#ifndef INCLUDED_GNURADIO_TAGS_HPP
#define INCLUDED_GNURADIO_TAGS_HPP

#include <gnuradio/gras.hpp>
#include <gruel/pmt.h>

namespace gnuradio
{

struct GRAS_API Tag
{

    //! the item \p tag occurred at (as a uint64_t)
    uint64_t offset;

    //! the key of \p tag (as a PMT symbol)
    pmt::pmt_t key;

    //! the value of \p tag (as a PMT)
    pmt::pmt_t value;

    //! the source ID of \p tag (as a PMT)
    pmt::pmt_t srcid;

    //! Comparison function to test which tag, \p x or \p y, came first in time
    static inline bool offset_compare(
        const Tag &x, const Tag &y
    ){
        return x.offset < y.offset;
    }
};

} //namespace gnuradio

#endif /*INCLUDED_GNURADIO_TAGS_HPP*/
