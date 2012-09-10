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
    size_t __pad; //avoid a probably non-issue w/ count
    SBufferConfig config;
};

inline void intrusive_ptr_add_ref(SBufferImpl *impl)
{
    ++impl->count;
}

inline void intrusive_ptr_release(SBufferImpl *impl)
{
    if (--impl->count == 0)
    {
        //call the deleter if possible
        boost::shared_ptr<SBufferDeleter> token_deleter = impl->config.token.lock();
        if (token_deleter)
        {
            SBuffer buff;
            (*token_deleter)(buff);
        }
        else if (impl->config.deleter)
        {
            SBuffer buff;
            impl->config.deleter(buff);
            impl->config.deleter = SBufferDeleter(); //reset deleter, so we dont double delete
        }
        else
        {
            delete impl; //its really dead now
        }
    }
}

inline SBufferConfig::SBufferConfig(void)
{
    memory = NULL;
    length = 0;
}

inline void *SBuffer::get_actual_memory(void) const
{
    return (*this)->config.memory;
}

inline size_t SBuffer::get_actual_length(void) const
{
    return (*this)->config.length;
}

inline void *SBuffer::get(void) const
{
    return ((char *)(*this)->config.memory) + this->offset;
}

inline Affinity SBuffer::get_affinity(void) const
{
    return (*this)->config.affinity;
}

inline bool SBuffer::unique(void) const
{
    return (*this)->count == 1;
}

inline size_t SBuffer::use_count(void) const
{
    return (*this)->count;
}

} //namespace gnuradio

#endif /*INCLUDED_GNURADIO_SBUFFER_IPP*/
