// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_LIBGRAS_IMPL_BITSET_HPP
#define INCLUDED_LIBGRAS_IMPL_BITSET_HPP

#include <gras/gras.hpp>
#include <boost/dynamic_bitset.hpp>

namespace gras
{

//! Its just dynamic bitset w/ the handle all() routine
struct BitSet : boost::dynamic_bitset<>
{
    GRAS_FORCE_INLINE bool all(void) const
    {
        return this->count() == this->size();
    }
};

} //namespace gras

#endif /*INCLUDED_LIBGRAS_IMPL_BITSET_HPP*/
