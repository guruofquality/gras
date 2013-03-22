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

    //! Convert a shared ptr of a derived class to an Element
    inline Element &Element::shared_to_element(void)
    {
        try
        {
            this->weak_self = this->shared_from_this();
        }
        catch(...){}
        return *this;
    }

} //namespace gras

#endif /*INCLUDED_GRAS_DETAIL_ELEMENT_HPP*/
