// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_ELEMENT_HPP
#define INCLUDED_GRAS_ELEMENT_HPP

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning (disable:4251)  // needs to have dll interface
#endif //_MSC_VER

#include <gras/gras.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace gras
{

struct ElementImpl;

typedef boost::shared_ptr<ElementImpl> ElementBase;

struct GRAS_API Element : ElementBase, boost::enable_shared_from_this<Element>
{

    //! Create an empty element
    Element(void);

    //! Creates a new element given the name
    Element(const std::string &name);

    //! An integer ID that is unique across the process
    long unique_id(void) const;

    //! Get the name of this element
    std::string name(void) const;

    //! get a canonical name for this element
    std::string to_string(void) const;

    /*******************************************************************
     * Compatibility for dealing with shared ptrs of Elements
     ******************************************************************/
    /*!
     * Create an element from a shared pointer to an element.
     * Good for that factory function/shared ptr paradigm.
     */
    template <typename T>
    inline Element(const boost::shared_ptr<T> &elem)
    {
        *this = elem->shared_to_element();
    }

    //! Convert a shared ptr of a derived class to an Element
    inline Element &shared_to_element(void)
    {
        try
        {
            this->weak_self = this->shared_from_this();
        }
        catch(...){}
        return *this;
    }

    //! for internal use only
    boost::weak_ptr<Element> weak_self;

};

} //namespace gras

#ifdef _MSC_VER
#pragma warning(pop)
#endif //_MSC_VER

#endif /*INCLUDED_GRAS_ELEMENT_HPP*/
