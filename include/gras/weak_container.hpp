// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_WEAK_CONTAINER_HPP
#define INCLUDED_GRAS_WEAK_CONTAINER_HPP

#include <gras/gras.hpp>
#include <boost/shared_ptr.hpp>

namespace gras
{

    /*!
     * Weak Container interface class:
     * A weak container is an object with a method called lock.
     * Lock gets a reference to an object's parent container.
     * The weak container itself does not actually hold a reference.
     * This container class is shared_ptr, a Pyobject, anything.
     */
    struct GRAS_API WeakContainer
    {
        WeakContainer(void);

        virtual ~WeakContainer(void);

        //! Lock creates a shared pointer holding a container reference
        virtual boost::shared_ptr<const void> lock(void) = 0;
    };

} //namespace gras

#endif /*INCLUDED_GRAS_WEAK_CONTAINER_HPP*/
