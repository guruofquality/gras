// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_GRAS_HPP
#define INCLUDED_GRAS_GRAS_HPP

#include <ciso646>

// http://gcc.gnu.org/wiki/Visibility
// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
  #define GRAS_HELPER_DLL_IMPORT __declspec(dllimport)
  #define GRAS_HELPER_DLL_EXPORT __declspec(dllexport)
  #define GRAS_HELPER_DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define GRAS_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
    #define GRAS_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
    #define GRAS_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define GRAS_HELPER_DLL_IMPORT
    #define GRAS_HELPER_DLL_EXPORT
    #define GRAS_HELPER_DLL_LOCAL
  #endif
#endif

#define GRAS_DLL //always build a dll

// Now we use the generic helper definitions above to define GRAS_API and GRAS_LOCAL.
// GRAS_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// GRAS_LOCAL is used for non-api symbols.

#ifdef GRAS_DLL // defined if GRAS is compiled as a DLL
  #ifdef GRAS_DLL_EXPORTS // defined if we are building the GRAS DLL (instead of using it)
    #define GRAS_API GRAS_HELPER_DLL_EXPORT
    #define GRAS_EXTERN
  #else
    #define GRAS_API GRAS_HELPER_DLL_IMPORT
    #define GRAS_EXTERN extern
  #endif // GRAS_DLL_EXPORTS
  #define GRAS_LOCAL GRAS_HELPER_DLL_LOCAL
#else // GRAS_DLL is not defined: this means GRAS is a static lib.
  #define GRAS_API
  #define GRAS_LOCAL
  #define GRAS_EXTERN
#endif // GRAS_DLL

#define GRAS_MAX_ALIGNMENT 64

//define cross platform attribute macros
#if defined(GRAS_DEBUG)
    #define GRAS_FORCE_INLINE         inline
#elif defined(BOOST_MSVC)
    #define GRAS_FORCE_INLINE         __forceinline
#elif defined(__GNUG__) && __GNUG__ >= 4
    #define GRAS_FORCE_INLINE         inline __attribute__((always_inline))
#else
    #define GRAS_FORCE_INLINE         inline
#endif

//want to have fun with branch prediction hints?
#ifdef __GNUC__
    #define GRAS_LIKELY(expr)    (__builtin_expect(bool(expr), 1))
    #define GRAS_UNLIKELY(expr)  (__builtin_expect(bool(expr), 0))
#else
    #define GRAS_LIKELY(expr)    (expr)
    #define GRAS_UNLIKELY(expr)  (expr)
#endif

//! A fixture for static initialization code
#define GRAS_STATIC_BLOCK(name) \
    static struct name ## _static_fixture__ \
    { \
        name ## _static_fixture__(void); \
    } name ## _static_fixture_instance__; \
    name ## _static_fixture__::name ## _static_fixture__(void)

namespace gras
{
    //! Typedef for absolute item indexes
    typedef unsigned long long item_index_t;

    //! Forward declare for element base class
    struct ElementImpl;

    //! Forward declare for block class
    struct Block;
}

#endif /*INCLUDED_GRAS_GRAS_HPP*/
