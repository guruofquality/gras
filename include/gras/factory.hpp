// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_FACTORY_HPP
#define INCLUDED_GRAS_FACTORY_HPP

#include <gras/gras.hpp>
#include <gras/element.hpp>
#include <PMC/PMC.hpp>
#include <string>

//! A fixture for static initialization code
#define GRAS_STATIC_BLOCK(name) \
    static struct name ## _static_fixture__ \
    { \
        name ## _static_fixture__(void); \
    } name ## _static_fixture_instance__; \
    name ## _static_fixture__::name ## _static_fixture__(void)

/*!
 * Register a block's factory function:
 * Declare this macro at the global scope in a cpp file.
 * The block will register at static initialization time.
 */
#define GRAS_REGISTER_FACTORY(name) \
    GRAS_STATIC_BLOCK(name) \
    {gras::Factory::register_make(#name, &name);}

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
class GRAS_API Factory : Callable
{
public:

    /*******************************************************************
     * Register API - don't look here, template magic, not helpful
     ******************************************************************/
    template <typename ReturnType>
    static void register_make(const std::string &name, ReturnType(*fcn)(void));

    template <typename ReturnType, typename Arg0>
    static void register_make(const std::string &name, ReturnType(*fcn)(const Arg0 &));

    template <typename ReturnType, typename Arg0, typename Arg1>
    static void register_make(const std::string &name, ReturnType(*fcn)(const Arg0 &, const Arg1 &));

    template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2>
    static void register_make(const std::string &name, ReturnType(*fcn)(const Arg0 &, const Arg1 &, const Arg2 &));

    /*******************************************************************
     * Make API - don't look here, template magic, not helpful
     ******************************************************************/
    inline
    static Element *make(const std::string &name);

    template <typename Arg0>
    static Element *make(const std::string &name, const Arg0 &a0);

    template <typename Arg0, typename Arg1>
    static Element *make(const std::string &name, const Arg0 &a0, const Arg1 &a1);

    template <typename Arg0, typename Arg1, typename Arg2>
    static Element *make(const std::string &name, const Arg0 &a0, const Arg1 &a1, const Arg2 &a2);

private:
    static void _register_make(const std::string &, void *);
    static Element *_make(const std::string &, const PMCC &);
};

}

#include <gras/detail/factory.hpp>

#endif /*INCLUDED_GRAS_FACTORY_HPP*/
