// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_DETAIL_ELEMENT_HPP
#define INCLUDED_GRAS_DETAIL_ELEMENT_HPP

namespace gras
{
    template <typename T>
    inline Element::Element(const boost::shared_ptr<T> &elem)
    {
        *this = elem->shared_to_element();
    }

} //namespace gras

#endif /*INCLUDED_GRAS_DETAIL_ELEMENT_HPP*/
