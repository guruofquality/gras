//
// Copyright 2012 Josh Blum
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef INCLUDED_GNURADIO_SBUFFER_HPP
#define INCLUDED_GNURADIO_SBUFFER_HPP

#include <gnuradio/gras.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/function.hpp>

namespace gnuradio
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

    /*!
     * Get a pointer to the start of the underlying memory
     */
    void *get_actual_memory(void) const;

    /*!
     * Get the length of the underlying memory in bytes
     */
    size_t get_actual_length(void) const;

    //! Get a pointer into valid memory
    void *get(const ptrdiff_t delta_bytes = 0) const;

    //! The offset into valid memory in bytes
    size_t offset;

    //! The number of valid bytes past offset
    size_t length;

    //! Get the affinity of the memory
    long get_affinity(void) const;

    //! Get the user index number
    size_t get_user_index(void) const;

    //! Unique if caller holds the only reference count
    bool unique(void) const;

    //! Get the number of reference holders
    size_t use_count(void) const;
};

} //namespace gnuradio

#include <gnuradio/sbuffer.ipp>

#endif /*INCLUDED_GNURADIO_SBUFFER_HPP*/
