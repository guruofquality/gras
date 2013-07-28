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
 *  gras::Factory::register_make("/proj/my_block", &make_my_block);
 *
 * Example call into the factory:
 *  gras::Element *my_block = gras::Factory::make("/proj/my_block", arg0, arg1);
 */
struct GRAS_API Factory
{
    /*******************************************************************
     * Register API - don't look here, template magic, not helpful
     ******************************************************************/
    template <typename ReturnType>
    static void register_make(const std::string &name, ReturnType(*fcn)());

    template <typename ReturnType, typename A0>
    static void register_make(const std::string &name, ReturnType(*fcn)(const A0 &));

    template <typename ReturnType, typename A0, typename A1>
    static void register_make(const std::string &name, ReturnType(*fcn)(const A0 &, const A1 &));

    template <typename ReturnType, typename A0, typename A1, typename A2>
    static void register_make(const std::string &name, ReturnType(*fcn)(const A0 &, const A1 &, const A2 &));

    template <typename ReturnType, typename A0, typename A1, typename A2, typename A3>
    static void register_make(const std::string &name, ReturnType(*fcn)(const A0 &, const A1 &, const A2 &, const A3 &));

    template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4>
    static void register_make(const std::string &name, ReturnType(*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &));

    template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
    static void register_make(const std::string &name, ReturnType(*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &));

    template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
    static void register_make(const std::string &name, ReturnType(*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &));

    template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
    static void register_make(const std::string &name, ReturnType(*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &));

    template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
    static void register_make(const std::string &name, ReturnType(*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &));

    template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
    static void register_make(const std::string &name, ReturnType(*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &, const A9 &));

    /*******************************************************************
     * Make API - don't look here, template magic, not helpful
     ******************************************************************/;
    inline
    static Element *make(const std::string &name);

    template <typename A0>
    static Element *make(const std::string &name, const A0 &);

    template <typename A0, typename A1>
    static Element *make(const std::string &name, const A0 &, const A1 &);

    template <typename A0, typename A1, typename A2>
    static Element *make(const std::string &name, const A0 &, const A1 &, const A2 &);

    template <typename A0, typename A1, typename A2, typename A3>
    static Element *make(const std::string &name, const A0 &, const A1 &, const A2 &, const A3 &);

    template <typename A0, typename A1, typename A2, typename A3, typename A4>
    static Element *make(const std::string &name, const A0 &, const A1 &, const A2 &, const A3 &, const A4 &);

    template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
    static Element *make(const std::string &name, const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &);

    template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
    static Element *make(const std::string &name, const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &);

    template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
    static Element *make(const std::string &name, const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &);

    template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
    static Element *make(const std::string &name, const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &);

    template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
    static Element *make(const std::string &name, const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &, const A9 &);

    /*******************************************************************
     * Private registration hooks
     ******************************************************************/
    static void _register_make(const std::string &, void *);
    static Element *_handle_make(const std::string &, const PMCC &);
};

}

#include <gras/detail/factory.hpp>

#endif /*INCLUDED_GRAS_FACTORY_HPP*/
