// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_WORK_BUFFER_HPP
#define INCLUDED_GRAS_WORK_BUFFER_HPP

#include <gras/gras.hpp>
#include <cstdlib>

namespace gras
{

template <typename PtrType>
struct WorkBuffer
{
    //! get a pointer of the desired type to this buffer
    template <typename T> T cast(void) const;

    //! get a native pointer type to this buffer
    PtrType get(void) const;

    //! Get the memory pointer reference
    PtrType &get(void);

    //! get the number of items in this buffer
    size_t size(void) const;

    //! Get the buffer length reference
    size_t &size(void);

    PtrType _mem;
    size_t _len;
};

template <typename PtrType> template <typename T>
inline T WorkBuffer<PtrType>::cast(void) const
{
    return reinterpret_cast<T>(_mem);
}

template <typename PtrType>
inline PtrType WorkBuffer<PtrType>::get(void) const
{
    return _mem;
}

template <typename PtrType>
inline PtrType &WorkBuffer<PtrType>::get(void)
{
    return _mem;
}

template <typename PtrType>
inline size_t WorkBuffer<PtrType>::size(void) const
{
    return _len;
}

template <typename PtrType>
inline size_t &WorkBuffer<PtrType>::size(void)
{
    return _len;
}

} //namespace gras

#endif /*INCLUDED_GRAS_WORK_BUFFER_HPP*/
