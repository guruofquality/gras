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
    template <typename ReturnType>
    static void register_make(const std::string &name, ReturnType(*fcn)());

    template <typename ReturnType, typename Arg0>
    static void register_make(const std::string &name, ReturnType(*fcn)(const Arg0 &));

    template <typename ReturnType, typename Arg0, typename Arg1>
    static void register_make(const std::string &name, ReturnType(*fcn)(const Arg0 &, const Arg1 &));

    template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2>
    static void register_make(const std::string &name, ReturnType(*fcn)(const Arg0 &, const Arg1 &, const Arg2 &));

    template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
    static void register_make(const std::string &name, ReturnType(*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &));

    template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    static void register_make(const std::string &name, ReturnType(*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &));

    template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    static void register_make(const std::string &name, ReturnType(*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &));

    template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
    static void register_make(const std::string &name, ReturnType(*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &));

    template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
    static void register_make(const std::string &name, ReturnType(*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &, const Arg7 &));

    template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
    static void register_make(const std::string &name, ReturnType(*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &, const Arg7 &, const Arg8 &));

    template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
    static void register_make(const std::string &name, ReturnType(*fcn)(const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &, const Arg7 &, const Arg8 &, const Arg9 &));

    /*******************************************************************
     * Make API - don't look here, template magic, not helpful
     ******************************************************************/;
    inline
    static Element *make(const std::string &name);

    template <typename Arg0>
    static Element *make(const std::string &name, const Arg0 &);

    template <typename Arg0, typename Arg1>
    static Element *make(const std::string &name, const Arg0 &, const Arg1 &);

    template <typename Arg0, typename Arg1, typename Arg2>
    static Element *make(const std::string &name, const Arg0 &, const Arg1 &, const Arg2 &);

    template <typename Arg0, typename Arg1, typename Arg2, typename Arg3>
    static Element *make(const std::string &name, const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &);

    template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    static Element *make(const std::string &name, const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &);

    template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    static Element *make(const std::string &name, const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &);

    template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
    static Element *make(const std::string &name, const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &);

    template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
    static Element *make(const std::string &name, const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &, const Arg7 &);

    template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
    static Element *make(const std::string &name, const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &, const Arg7 &, const Arg8 &);

    template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
    static Element *make(const std::string &name, const Arg0 &, const Arg1 &, const Arg2 &, const Arg3 &, const Arg4 &, const Arg5 &, const Arg6 &, const Arg7 &, const Arg8 &, const Arg9 &);

    /*******************************************************************
     * Private registration hooks
     ******************************************************************/
    static void _register_make(const std::string &, void *);
    static Element *_handle_make(const std::string &, const PMCC &);
};

}

#include <gras/detail/factory.hpp>

#endif /*INCLUDED_GRAS_FACTORY_HPP*/
