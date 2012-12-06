// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_TAG_ITER_HPP
#define INCLUDED_GRAS_TAG_ITER_HPP

#include <gras/gras.hpp>
#include <boost/range.hpp> //iterator range
#include <vector>

namespace gras
{

    //! forward declared Tag type
    struct Tag;

    //! Iterator return type stl and boost compliant
    typedef boost::iterator_range<std::vector<Tag>::const_iterator> TagIter;

} //namespace gras

#endif /*INCLUDED_GRAS_TAG_ITER_HPP*/
