// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_DETAIL_ELEMENT_HPP
#define INCLUDED_GRAS_DETAIL_ELEMENT_HPP

#include <boost/weak_ptr.hpp>

namespace gras
{

    //! Weak element overload for the case of shared_ptr container
    struct WeakElementSharedPtr : WeakElement
    {
        WeakElementSharedPtr(boost::weak_ptr<const void> weak_self)
        {
            _weak_self = weak_self;
        }
        boost::shared_ptr<const void> lock(void)
        {
            return _weak_self.lock();
        }
        boost::weak_ptr<const void> _weak_self;
    };

    template <typename T>
    inline Element::Element(const boost::shared_ptr<T> &elem)
    {
        //the container is a shared pointer, so save the reference,
        //unless the reference was already set to something
        if (not elem->weak_self)
        {
            elem->weak_self.reset(new WeakElementSharedPtr(elem));
        }

        //initialize this new Element from the argument passed
        *this = *elem;
    }

} //namespace gras

#endif /*INCLUDED_GRAS_DETAIL_ELEMENT_HPP*/
