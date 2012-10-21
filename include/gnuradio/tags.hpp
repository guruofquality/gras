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

#ifndef INCLUDED_GNURADIO_TAGS_HPP
#define INCLUDED_GNURADIO_TAGS_HPP

#include <gnuradio/gras.hpp>
#include <boost/operators.hpp>

//TODO -- this is a stub for the PMC libray
#include <gruel/pmt.h>
typedef pmt::pmt_t PMC;
typedef pmt::pmt_t PMCC;

namespace gnuradio
{

struct GRAS_API Tag : boost::less_than_comparable<Tag>
{
    //! Make an empty tag with null members
    Tag(void);

    //! Make a tag from parameters to initialize the members
    Tag(const uint64_t &offset, const PMCC &key, const PMCC &value, const PMCC &srcid = PMCC());

    //! the absolute item count associated with this tag
    uint64_t offset;

    //! A symbolic name identifying the type of tag
    PMCC key;

    //! The value of this tag -> the sample metadata
    PMCC value;

    //! The optional source ID -> something unique
    PMCC srcid;
};

GRAS_API bool operator<(const Tag &lhs, const Tag &rhs);

} //namespace gnuradio

#endif /*INCLUDED_GNURADIO_TAGS_HPP*/
