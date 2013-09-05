// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_DETAIL_SBUFFER_HPP
#define INCLUDED_GRAS_DETAIL_SBUFFER_HPP

#include <boost/detail/atomic_count.hpp>

namespace gras
{

extern GRAS_API void intrusive_ptr_add_ref(SBufferImpl *impl);
extern GRAS_API void intrusive_ptr_release(SBufferImpl *impl);

struct SBufferImpl
{
    SBufferImpl(const SBufferConfig &config);
    boost::detail::atomic_count count;
    SBufferConfig config;
};

GRAS_FORCE_INLINE const void *SBuffer::get_actual_memory(void) const
{
    return (*this)->config.memory;
}

GRAS_FORCE_INLINE void *SBuffer::get_actual_memory(void)
{
    return (*this)->config.memory;
}

GRAS_FORCE_INLINE size_t SBuffer::get_actual_length(void) const
{
    return (*this)->config.length;
}

GRAS_FORCE_INLINE const void *SBuffer::get(const ptrdiff_t delta_bytes) const
{
    return ((char *)(*this)->config.memory) + this->offset + delta_bytes;
}

GRAS_FORCE_INLINE void *SBuffer::get(const ptrdiff_t delta_bytes)
{
    return ((char *)(*this)->config.memory) + this->offset + delta_bytes;
}

GRAS_FORCE_INLINE long SBuffer::get_affinity(void) const
{
    return (*this)->config.affinity;
}

GRAS_FORCE_INLINE size_t SBuffer::get_user_index(void) const
{
    return (*this)->config.user_index;
}

GRAS_FORCE_INLINE bool SBuffer::unique(void) const
{
    return (*this)->count == 1;
}

GRAS_FORCE_INLINE size_t SBuffer::use_count(void) const
{
    return (*this)->count;
}

} //namespace gras

#endif /*INCLUDED_GRAS_DETAIL_SBUFFER_HPP*/
