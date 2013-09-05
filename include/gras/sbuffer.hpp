// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_SBUFFER_HPP
#define INCLUDED_GRAS_SBUFFER_HPP

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning (disable:4251)  // needs to have dll interface
#endif //_MSC_VER

#include <gras/gras.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/function.hpp>

namespace gras
{

struct SBufferImpl;
struct SBuffer;

//! The callback function type when buffers dereference
typedef boost::function<void(SBuffer &)> SBufferDeleter;

//! The token type held by the caller
typedef boost::shared_ptr<SBufferDeleter> SBufferToken;

//! The token type weak ptr help by buffer
typedef boost::weak_ptr<SBufferDeleter> SBufferTokenWeak;

struct GRAS_API SBufferConfig
{
    //! Default constructor zeros out buffer config
    SBufferConfig(void);

    //! Destructor
    ~SBufferConfig(void);

    //! pointer to the memory start
    void *memory;

    //! length of the memory in bytes
    size_t length;

    //! memory affinity - meta information
    long affinity;

    //! index number for custom allocation purposes
    size_t user_index;

    //! deleter callback, may be used to free
    SBufferDeleter deleter;

    //! token object, called if set under deref condition
    SBufferTokenWeak token;
};

/*!
 * SBuffer is a smart/shared reference counted handler of memory.
 * Thank you boost smart/shared pointers for the disambiguation!
 *
 * Default allocator:
 * To use the default system allocator, set the memory to NULL,
 * and set length and affinity to the desired values.
 * The deleter will be automaically configured by the allocator.
 *
 * Custom allocator:
 * Set all config struct members. Its all you!
 *
 * Token usage:
 * When a token is set, the buffer will not cleanup and call the deleter.
 * Rather, the bound function in the token will be called with the buffer.
 * A parent object should hold the shared pointer to the token.
 * When the parent object deconstructs, the weak pointer will die,
 * and the normal buffer cleanup/freeing/deconstruction will happen.
 *
 * Length and offset usage:
 * Length and offset are intentionally object members
 * and not part of the ref-counted intrusive_ptr guts.
 * These members should be modified carefully
 * to pass a subset of the memory downstream.
 *
 * Length and offset recommendation:
 * These values should probably be reset by the
 * bound token or when a fresh buffer is popped.
 */
struct GRAS_API SBuffer : boost::intrusive_ptr<SBufferImpl>
{
    //! Default constructor, zeros things
    SBuffer(void);

    /*!
     * Create a new buffer.
     * The config object represents a chunk of memory,
     * or instructions for the default allocator.
     */
    SBuffer(const SBufferConfig &config);

    //! Destructor
    ~SBuffer(void);

    /*!
     * Get a pointer to the start of the underlying memory
     */
    const void *get_actual_memory(void) const;

    /*!
     * Get a pointer to the start of the underlying memory
     */
    void *get_actual_memory(void);

    /*!
     * Get the length of the underlying memory in bytes
     */
    size_t get_actual_length(void) const;

    //! Get a pointer into valid memory
    const void *get(const ptrdiff_t delta_bytes = 0) const;

    //! Get a pointer into valid memory
    void *get(const ptrdiff_t delta_bytes = 0);

    //! The offset into valid memory in bytes
    size_t offset;

    //! The number of valid bytes past offset
    size_t length;

    //! Pointer to the end of the last buffer
    void *last;

    //! Get the affinity of the memory
    long get_affinity(void) const;

    //! Get the user index number
    size_t get_user_index(void) const;

    //! Unique if caller holds the only reference count
    bool unique(void) const;

    //! Get the number of reference holders
    size_t use_count(void) const;
};

} //namespace gras

#include <gras/detail/sbuffer.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#endif //_MSC_VER

#endif /*INCLUDED_GRAS_SBUFFER_HPP*/
