// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_DETAIL_ELEMENT_HPP
#define INCLUDED_GRAS_DETAIL_ELEMENT_HPP

namespace gras
{
    struct WeakElementSharedPtr : WeakElement
    {
        WeakElementSharedPtr(boost::weak_ptr<Element> weak_self)
        {
            _weak_self = weak_self;
        }
        boost::shared_ptr<void> lock(void)
        {
            return _weak_self.lock();
        }
        boost::weak_ptr<Element> _weak_self;
    };

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
            if (not this->weak_self)
                this->weak_self.reset(new WeakElementSharedPtr(this->shared_from_this()));
        }
        catch(...){}
        return *this;
    }

} //namespace gras

#endif /*INCLUDED_GRAS_DETAIL_ELEMENT_HPP*/
