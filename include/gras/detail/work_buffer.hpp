// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_DETAIL_WORK_BUFFER_HPP
#define INCLUDED_GRAS_DETAIL_WORK_BUFFER_HPP

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

template <typename PtrType>
inline const std::vector<PtrType> &WorkBufferArray<PtrType>::vec(void) const
{
    return _vec;
}

template <typename PtrType>
inline std::vector<PtrType> &WorkBufferArray<PtrType>::vec(void)
{
    return _vec;
}

template <typename PtrType>
inline void WorkBufferArray<PtrType>::resize(const size_t size)
{
    _vec.resize(size);
    std::vector<WorkBuffer<PtrType> >::resize(size);
}

} //namespace gras

#endif /*INCLUDED_GRAS_DETAIL_WORK_BUFFER_HPP*/
