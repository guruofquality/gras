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

struct Block;

struct GRAS_API Element : ElementBase, boost::enable_shared_from_this<Element>
{
    //! Create an empty element
    Element(void);

    //! Creates a new element given the name
    Element(const std::string &name);

    /*virtual*/ ~Element(void);

    /*******************************************************************
     * identification interface
     ******************************************************************/

    //! An integer ID that is unique across the process
    long unique_id(void) const;

    //! Get the name of this element
    std::string name(void) const;

    //! get a canonical name for this element
    std::string to_string(void) const;

    /*******************************************************************
     * element tree interface
     ******************************************************************/

    /*!
     * Adopt an element as a child under the given name.
     *
     * This API allows the user to structure a hierarchy of elements.
     * This element will become the parent of the child element.
     *
     * \param name the name of the child node
     * \param child an element to be adopted
     */
    void adopt_element(const std::string &name, const Element &child);

    /*!
     * Locate a block in the element tree hierarchy.
     *
     * Paths are unix style, absolte and relatives paths are possible.
     * This call throws an invalid argument when bad paths are given.
     *
     * Example path: /my_hier_block/my_block0
     * Example path: ../my_block1
     *
     * \param path a path to a block (leaf) in the tree
     * \return a pointer to the block
     */
    Block *locate_block(const std::string &path);

    /*******************************************************************
     * Compatibility for dealing with shared ptrs of Elements
     ******************************************************************/
    /*!
     * Create an element from a shared pointer to an element.
     * Good for that factory function/shared ptr paradigm.
     */
    template <typename T>
    inline Element(const boost::shared_ptr<T> &elem);

    //! Convert a shared ptr of a derived class to an Element
    inline Element &shared_to_element(void);

    //! for internal use only
    boost::weak_ptr<Element> weak_self;

};

} //namespace gras

#include <gras/detail/element.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#endif //_MSC_VER

#endif /*INCLUDED_GRAS_ELEMENT_HPP*/
