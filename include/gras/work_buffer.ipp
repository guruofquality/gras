// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_WORK_BUFFER_IPP
#define INCLUDED_GRAS_WORK_BUFFER_IPP

namespace gras
{

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

template <typename PtrType>
inline size_t WorkBufferArray<PtrType>::min(void) const
{
    return _min;
}

template <typename PtrType>
inline size_t &WorkBufferArray<PtrType>::min(void)
{
    return _min;
}

template <typename PtrType>
inline size_t WorkBufferArray<PtrType>::max(void) const
{
    return _max;
}

template <typename PtrType>
inline size_t &WorkBufferArray<PtrType>::max(void)
{
    return _max;
}

} //namespace gras

#endif /*INCLUDED_GRAS_WORK_BUFFER_IPP*/
