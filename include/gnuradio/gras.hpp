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
// along with io_sig program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef INCLUDED_GNURADIO_GRAS_HPP
#define INCLUDED_GNURADIO_GRAS_HPP

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/intrusive_ptr.hpp>

//this is part of core for now, treat it as such
#include <gr_core_api.h>
#define GRAS_API GR_CORE_API

#define GRAS_MAX_ALIGNMENT 32

namespace gnuradio
{

struct ElementImpl;

struct SBuffer;
struct SBufferImpl;

void intrusive_ptr_add_ref(SBufferImpl*);
void intrusive_ptr_release(SBufferImpl*);

struct Affinity
{
    Affinity(void)
    {
        value = -1;
    }

    operator long long(void) const
    {
        return value;
    }

    long long value;
};

} //namespace gnuradio

#endif /*INCLUDED_GNURADIO_GRAS_HPP*/
