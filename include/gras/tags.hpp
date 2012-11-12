// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_TAGS_HPP
#define INCLUDED_GRAS_TAGS_HPP

#include <gras/gras.hpp>
#include <boost/operators.hpp>
#include <PMC/PMC.hpp>

namespace gras
{

struct GRAS_API Tag : boost::less_than_comparable<Tag>
{
    //! Make an empty tag with null members
    Tag(void);

    //! Make a tag from parameters to initialize the members
    Tag(const item_index_t &offset, const PMCC &key, const PMCC &value, const PMCC &srcid = PMCC());

    //! the absolute item count associated with this tag
    item_index_t offset;

    //! A symbolic name identifying the type of tag
    PMCC key;

    //! The value of this tag -> the sample metadata
    PMCC value;

    //! The optional source ID -> something unique
    PMCC srcid;
};

GRAS_API bool operator<(const Tag &lhs, const Tag &rhs);

} //namespace gras

#include <boost/range.hpp> //iterator range
#include <vector>

namespace gras
{
    //! Iterator return type stl and boost compliant
    typedef boost::iterator_range<std::vector<Tag>::const_iterator> TagIter;

} //namespace gras

#endif /*INCLUDED_GRAS_TAGS_HPP*/
