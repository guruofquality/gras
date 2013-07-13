// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_FACTORY_HPP
#define INCLUDED_GRAS_FACTORY_HPP

#include <gras/gras.hpp>
#include <gras/element.hpp>
#include <PMC/PMC.hpp>
#include <string>

/*!
 * Register a block's factory function:
 * Declare this macro at the global scope in a cpp file.
 * The block will register at static initialization time.
 */
#define GRAS_REGISTER_FACTORY(name, fcn) \
    GRAS_STATIC_BLOCK(fcn) \
    {gras::Factory::register_make(name, &fcn);}

namespace gras
{

/*!
 * Element factory:
 *  - Register factory functions into the global factory.
 *  - Call make() to create element from global factory.
 *
 * Example register a factory function:
 *  gras::Factory::register_make("make_my_block", &make_my_block);
 *
 * Example call into the factory:
 *  gras::Element *my_block = gras::Factory::make("make_my_block", arg0, arg1);
 */
struct GRAS_API Factory
{
    /*******************************************************************
     * Register API - don't look here, template magic, not helpful
     ******************************************************************/
    #for $NARGS in range($MAX_ARGS)
    template <typename ReturnType, $expand('typename A%d', $NARGS)>
    static void register_make(const std::string &name, ReturnType(*fcn)($expand('const A%d &', $NARGS)));

    #end for
    /*******************************************************************
     * Make API - don't look here, template magic, not helpful
     ******************************************************************/;
    #for $NARGS in range($MAX_ARGS)
    template <$expand('typename A%d', $NARGS)>
    static Element *make(const std::string &name, $expand('const A%d &', $NARGS));

    #end for
    /*******************************************************************
     * Private registration hooks
     ******************************************************************/
    static void _register_make(const std::string &, void *);
    static Element *_handle_make(const std::string &, const PMCC &);
};

}

#include <gras/detail/factory.hpp>

#endif /*INCLUDED_GRAS_FACTORY_HPP*/
