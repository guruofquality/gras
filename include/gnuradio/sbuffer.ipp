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

#ifndef INCLUDED_GNURADIO_SBUFFER_IPP
#define INCLUDED_GNURADIO_SBUFFER_IPP

#include <boost/detail/atomic_count.hpp>

namespace gnuradio
{

struct SBufferImpl
{
    SBufferImpl(const SBufferConfig &config):
        count(0),
        config(config)
    {
        //NOP
    }

    boost::detail::atomic_count count;
    SBufferConfig config;
};


extern GRAS_API void sbuffer_handle_deref(SBufferImpl *impl);

GRAS_FORCE_INLINE void intrusive_ptr_add_ref(SBufferImpl *impl)
{
    ++impl->count;
}

GRAS_FORCE_INLINE void intrusive_ptr_release(SBufferImpl *impl)
{
    if (--impl->count == 0)
    {
        sbuffer_handle_deref(impl);
    }
}

GRAS_FORCE_INLINE SBuffer::SBuffer(void):
    offset(0),
    length(0)
{
    //NOP
}

GRAS_FORCE_INLINE void *SBuffer::get_actual_memory(void) const
{
    return (*this)->config.memory;
}

GRAS_FORCE_INLINE size_t SBuffer::get_actual_length(void) const
{
    return (*this)->config.length;
}

GRAS_FORCE_INLINE void *SBuffer::get(const ptrdiff_t delta_bytes) const
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

} //namespace gnuradio

#endif /*INCLUDED_GNURADIO_SBUFFER_IPP*/
