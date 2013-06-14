// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_LIBGRAS_IMPL_DEBUG_HPP
#define INCLUDED_LIBGRAS_IMPL_DEBUG_HPP

//#define MALLOC_DEBUG
#ifdef MALLOC_DEBUG
#include <stdexcept>
extern void *operator new(std::size_t n) throw (std::bad_alloc);
#endif

#include <iostream>
#include <stdexcept>
#include <boost/current_function.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>

//----------------------------------------------------------------------
//-- set to 1 to enable these debugs:
//----------------------------------------------------------------------
#define GENESIS 0
#define ARMAGEDDON 0
#define DONE_PRINTS 0

//----------------------------------------------------------------------
//-- define to enable these debugs:
//----------------------------------------------------------------------
#define ASSERTING
//#define MESSAGE_TRACING
//#define ITEM_CONSPROD

//----------------------------------------------------------------------
//-- time accumulation printer
//----------------------------------------------------------------------
#define TIME_ACCUM_PRINT(code) \
{\
    static boost::posix_time::time_duration __t; \
    const boost::system_time __t0 = boost::get_system_time(); \
    {code} \
    const boost::system_time __t1 = boost::get_system_time(); \
    __t += __t1 - __t0; \
    std::cerr << __FILE__ << ":" << __LINE__ << " total time: " << boost::posix_time::to_simple_string(__t) << std::endl; \
}

//----------------------------------------------------------------------
//-- various debug prints
//----------------------------------------------------------------------
#define HERE() std::cerr << __FILE__ << ":" << __LINE__ << std::endl << std::flush;
#define VAR(x) std::cerr << #x << " = " << (x) << std::endl << std::flush;

#ifdef MESSAGE_TRACING
#define MESSAGE_TRACER() std::cerr << name << " in " << BOOST_CURRENT_FUNCTION << std::endl << std::flush;
#else
#define MESSAGE_TRACER()
#endif

//----------------------------------------------------------------------
//-- implementation for assert debug
//----------------------------------------------------------------------
#ifdef ASSERTING
#define ASSERT(x) {if GRAS_UNLIKELY(not (x)) \
{ \
    std::cerr << "ASSERT FAIL " << __FILE__ << ":" << __LINE__ << "\n\t" << #x << std::endl << std::flush; \
    throw std::runtime_error(std::string("ASSERT FAIL ") + #x); \
}}
#else
#define ASSERT(x) {}
#endif

#endif /*INCLUDED_LIBGRAS_IMPL_DEBUG_HPP*/
