// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_FACTORY_HPP
#define INCLUDED_GRAS_FACTORY_HPP

#include <gras/gras.hpp>
#include <gras/element.hpp>
#include <PMC/PMC.hpp>
#include <vector>
#include <string>

namespace gras
{

/*!
 * The just in time factory:
 * Compile a C++ source and load it into the element factory.
 *
 * Flags are an optional list of compiler flags.
 * See the man page for clang for possible options.
 * Example: flags.push_back("-O3")
 *
 * Include directories control the header file search path.
 * Users may leave this empty unless headers
 * are installed into non-standard directories.
 *
 * \param source C++ source code in a string
 * \param flags optional compiler flags
 */
GRAS_API void jit_factory(const std::string &source, const std::vector<std::string> &flags);

/***********************************************************************
 * Register API - don't look here, template magic, not helpful
 * Example register a factory function:
 *   gras::register_factory("/proj/my_block", &make_my_block);
 **********************************************************************/
template <typename ReturnType>
static void register_factory(const std::string &path, ReturnType(*fcn)());

template <typename ReturnType, typename A0>
static void register_factory(const std::string &path, ReturnType(*fcn)(const A0 &));

template <typename ReturnType, typename A0, typename A1>
static void register_factory(const std::string &path, ReturnType(*fcn)(const A0 &, const A1 &));

template <typename ReturnType, typename A0, typename A1, typename A2>
static void register_factory(const std::string &path, ReturnType(*fcn)(const A0 &, const A1 &, const A2 &));

template <typename ReturnType, typename A0, typename A1, typename A2, typename A3>
static void register_factory(const std::string &path, ReturnType(*fcn)(const A0 &, const A1 &, const A2 &, const A3 &));

template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4>
static void register_factory(const std::string &path, ReturnType(*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &));

template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
static void register_factory(const std::string &path, ReturnType(*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &));

template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
static void register_factory(const std::string &path, ReturnType(*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &));

template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
static void register_factory(const std::string &path, ReturnType(*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &));

template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
static void register_factory(const std::string &path, ReturnType(*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &));

template <typename ReturnType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
static void register_factory(const std::string &path, ReturnType(*fcn)(const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &, const A9 &));

/***********************************************************************
 * Make API - don't look here, template magic, not helpful
 * Example call into the factory:
 *   gras::Element *my_block = gras::make("/proj/my_block", arg0, arg1);
 **********************************************************************/
inline
static Element *make(const std::string &path);

template <typename A0>
static Element *make(const std::string &path, const A0 &);

template <typename A0, typename A1>
static Element *make(const std::string &path, const A0 &, const A1 &);

template <typename A0, typename A1, typename A2>
static Element *make(const std::string &path, const A0 &, const A1 &, const A2 &);

template <typename A0, typename A1, typename A2, typename A3>
static Element *make(const std::string &path, const A0 &, const A1 &, const A2 &, const A3 &);

template <typename A0, typename A1, typename A2, typename A3, typename A4>
static Element *make(const std::string &path, const A0 &, const A1 &, const A2 &, const A3 &, const A4 &);

template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
static Element *make(const std::string &path, const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &);

template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
static Element *make(const std::string &path, const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &);

template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
static Element *make(const std::string &path, const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &);

template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
static Element *make(const std::string &path, const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &);

template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
static Element *make(const std::string &path, const A0 &, const A1 &, const A2 &, const A3 &, const A4 &, const A5 &, const A6 &, const A7 &, const A8 &, const A9 &);


//! Register factory functions into the global factory.
GRAS_API void _register_factory(const std::string &, void *);

//! Call make() to create element from global factory.
GRAS_API Element *_handle_make(const std::string &, const PMCC &);
}

/*!
 * Register a block's factory function:
 * Declare this macro at the global scope in a cpp file.
 * The block will register at static initialization time.
 */
#define GRAS_REGISTER_FACTORY(path, fcn) \
    GRAS_STATIC_BLOCK(fcn) \
    {gras::register_factory(path, &fcn);}

/*!
 * Register a block's constructor into the factory:
 * The arguments to this macro must be the types of each constructor argument.
 * Example: GRAS_REGISTER_FACTORY2("/proj/my_block", MyBlock, std::string, size_t)
 * Declare this macro at the global scope in a cpp file.
 * The block will register at static initialization time.
 */
#define GRAS_REGISTER_FACTORY0(path, type) \
    static gras::Element *make_ ## type() \
    { return new type(); } \
    GRAS_REGISTER_FACTORY(path, make_##type)

/*!
 * Register a block's constructor into the factory:
 * The arguments to this macro must be the types of each constructor argument.
 * Example: GRAS_REGISTER_FACTORY2("/proj/my_block", MyBlock, std::string, size_t)
 * Declare this macro at the global scope in a cpp file.
 * The block will register at static initialization time.
 */
#define GRAS_REGISTER_FACTORY1(path, type, A0) \
    static gras::Element *make_ ## type(const A0 &a0) \
    { return new type(a0); } \
    GRAS_REGISTER_FACTORY(path, make_##type)

/*!
 * Register a block's constructor into the factory:
 * The arguments to this macro must be the types of each constructor argument.
 * Example: GRAS_REGISTER_FACTORY2("/proj/my_block", MyBlock, std::string, size_t)
 * Declare this macro at the global scope in a cpp file.
 * The block will register at static initialization time.
 */
#define GRAS_REGISTER_FACTORY2(path, type, A0, A1) \
    static gras::Element *make_ ## type(const A0 &a0, const A1 &a1) \
    { return new type(a0, a1); } \
    GRAS_REGISTER_FACTORY(path, make_##type)

/*!
 * Register a block's constructor into the factory:
 * The arguments to this macro must be the types of each constructor argument.
 * Example: GRAS_REGISTER_FACTORY2("/proj/my_block", MyBlock, std::string, size_t)
 * Declare this macro at the global scope in a cpp file.
 * The block will register at static initialization time.
 */
#define GRAS_REGISTER_FACTORY3(path, type, A0, A1, A2) \
    static gras::Element *make_ ## type(const A0 &a0, const A1 &a1, const A2 &a2) \
    { return new type(a0, a1, a2); } \
    GRAS_REGISTER_FACTORY(path, make_##type)

/*!
 * Register a block's constructor into the factory:
 * The arguments to this macro must be the types of each constructor argument.
 * Example: GRAS_REGISTER_FACTORY2("/proj/my_block", MyBlock, std::string, size_t)
 * Declare this macro at the global scope in a cpp file.
 * The block will register at static initialization time.
 */
#define GRAS_REGISTER_FACTORY4(path, type, A0, A1, A2, A3) \
    static gras::Element *make_ ## type(const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3) \
    { return new type(a0, a1, a2, a3); } \
    GRAS_REGISTER_FACTORY(path, make_##type)

/*!
 * Register a block's constructor into the factory:
 * The arguments to this macro must be the types of each constructor argument.
 * Example: GRAS_REGISTER_FACTORY2("/proj/my_block", MyBlock, std::string, size_t)
 * Declare this macro at the global scope in a cpp file.
 * The block will register at static initialization time.
 */
#define GRAS_REGISTER_FACTORY5(path, type, A0, A1, A2, A3, A4) \
    static gras::Element *make_ ## type(const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4) \
    { return new type(a0, a1, a2, a3, a4); } \
    GRAS_REGISTER_FACTORY(path, make_##type)

/*!
 * Register a block's constructor into the factory:
 * The arguments to this macro must be the types of each constructor argument.
 * Example: GRAS_REGISTER_FACTORY2("/proj/my_block", MyBlock, std::string, size_t)
 * Declare this macro at the global scope in a cpp file.
 * The block will register at static initialization time.
 */
#define GRAS_REGISTER_FACTORY6(path, type, A0, A1, A2, A3, A4, A5) \
    static gras::Element *make_ ## type(const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5) \
    { return new type(a0, a1, a2, a3, a4, a5); } \
    GRAS_REGISTER_FACTORY(path, make_##type)

/*!
 * Register a block's constructor into the factory:
 * The arguments to this macro must be the types of each constructor argument.
 * Example: GRAS_REGISTER_FACTORY2("/proj/my_block", MyBlock, std::string, size_t)
 * Declare this macro at the global scope in a cpp file.
 * The block will register at static initialization time.
 */
#define GRAS_REGISTER_FACTORY7(path, type, A0, A1, A2, A3, A4, A5, A6) \
    static gras::Element *make_ ## type(const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6) \
    { return new type(a0, a1, a2, a3, a4, a5, a6); } \
    GRAS_REGISTER_FACTORY(path, make_##type)

/*!
 * Register a block's constructor into the factory:
 * The arguments to this macro must be the types of each constructor argument.
 * Example: GRAS_REGISTER_FACTORY2("/proj/my_block", MyBlock, std::string, size_t)
 * Declare this macro at the global scope in a cpp file.
 * The block will register at static initialization time.
 */
#define GRAS_REGISTER_FACTORY8(path, type, A0, A1, A2, A3, A4, A5, A6, A7) \
    static gras::Element *make_ ## type(const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7) \
    { return new type(a0, a1, a2, a3, a4, a5, a6, a7); } \
    GRAS_REGISTER_FACTORY(path, make_##type)

/*!
 * Register a block's constructor into the factory:
 * The arguments to this macro must be the types of each constructor argument.
 * Example: GRAS_REGISTER_FACTORY2("/proj/my_block", MyBlock, std::string, size_t)
 * Declare this macro at the global scope in a cpp file.
 * The block will register at static initialization time.
 */
#define GRAS_REGISTER_FACTORY9(path, type, A0, A1, A2, A3, A4, A5, A6, A7, A8) \
    static gras::Element *make_ ## type(const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7, const A8 &a8) \
    { return new type(a0, a1, a2, a3, a4, a5, a6, a7, a8); } \
    GRAS_REGISTER_FACTORY(path, make_##type)

/*!
 * Register a block's constructor into the factory:
 * The arguments to this macro must be the types of each constructor argument.
 * Example: GRAS_REGISTER_FACTORY2("/proj/my_block", MyBlock, std::string, size_t)
 * Declare this macro at the global scope in a cpp file.
 * The block will register at static initialization time.
 */
#define GRAS_REGISTER_FACTORY10(path, type, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) \
    static gras::Element *make_ ## type(const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7, const A8 &a8, const A9 &a9) \
    { return new type(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9); } \
    GRAS_REGISTER_FACTORY(path, make_##type)

#include <gras/detail/factory.hpp>

#endif /*INCLUDED_GRAS_FACTORY_HPP*/
