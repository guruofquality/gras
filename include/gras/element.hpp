// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_ELEMENT_HPP
#define INCLUDED_GRAS_ELEMENT_HPP

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning (disable:4251)  // needs to have dll interface
#endif //_MSC_VER

#include <gras/gras.hpp>
#include <gras/callable.hpp>
#include <gras/block_config.hpp>
#include <gras/weak_container.hpp>
#include <boost/shared_ptr.hpp>

namespace gras
{

/*!
 * Element is a base class for all topological elements.
 */
struct GRAS_API Element : Callable, boost::shared_ptr<ElementImpl>
{
    //! Create an empty element
    Element(void);

    //! Creates a new element given the name
    Element(const std::string &name);

    /*!
     * Create an element from a shared pointer to an element.
     * Good for that factory function/shared ptr paradigm.
     */
    template <typename T>
    Element(const boost::shared_ptr<T> &elem);

    virtual ~Element(void);

    //! Convert this object to the element base class
    const Element &to_element(void) const;

    //! Convert this object to the element base class
    Element &to_element(void);

    /*!
     * Set a reference wrapper for the parent container to this element.
     * Allows internals to get a reference to the container holding an element.
     * The actual container could be a shared_ptr or perhaps a Python object.
     * Note: the container parameter is assumed to be allocated with new.
     * The element object will be responsible for deleting container.
     */
    void set_container(WeakContainer *container);

    /*!
     * Check if another element is the same as this one.
     * \return true if the rhs is the same as *this.
     */
    bool equals(const Element &rhs);

    /*!
     * Get a canonical name for this element.
     * This is used for printing elements for debug purposes.
     * Do not rely on the string to have any specific formatting.
     * \return a canonical string representation
     */
    std::string to_string(void) const;

    /*******************************************************************
     * config interface
     ******************************************************************/

    //! Get the global block config settings
    const GlobalBlockConfig &global_config(void) const;

    //! Get the global block config settings
    GlobalBlockConfig &global_config(void);

    /*!
     * Commit changes to the global configuration.
     * Call this after modifying the global config.
     * Must be call to apply changes to the global config.
     */
    virtual void commit_config(void);

    /*******************************************************************
     * identification interface
     ******************************************************************/

    /*!
     * Set the unique identifier of this element.
     * The UID must be unique across the process.
     * Typically the user will set a UID so an element
     * will have a known indentification for the query client.
     * This call will throw if UID is not process-unique.
     * \param uid a new unique identifier for this element
     */
    void set_uid(const std::string &uid);

    /*!
     * Get the unique identifier of this element.
     * Typically, a name is given to an element on construction.
     * By default UID will be the name or name + some hash
     * when there is more than one element of the same name.
     * Otherwise UID is set the by the set_uid API call.
     * \return the element's unique identifier as a string
     */
    std::string get_uid(void) const;

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
     * Locate an element in the element tree hierarchy.
     *
     * Paths are unix style, absolte and relatives paths are possible.
     * This call throws an invalid argument when bad paths are given.
     *
     * Example path: /my_hier_block/my_block0
     * Example path: ../my_block1
     *
     * \param path a path to an element (leaf) in the tree
     * \return a pointer to the element object
     */
    Element *locate_element(const std::string &path);

};

} //namespace gras

#include <gras/detail/element.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#endif //_MSC_VER

#endif /*INCLUDED_GRAS_ELEMENT_HPP*/
