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

#ifndef INCLUDED_LIBGRAS_ALLOC_ON_NODE_HPP
#define INCLUDED_LIBGRAS_ALLOC_ON_NODE_HPP

#include <Theron/Assert.h>
#include <Theron/Detail/Threading/Utils.h>

//----------------------------------------------------------------------
//-- a little cross platform numa allocator
//-- use the existing theron defines for convenience
//----------------------------------------------------------------------
inline void *AllocOnNode(const long node, const size_t size)
{

#if THERON_NUMA

#if THERON_WINDOWS

    #if _WIN32_WINNT >= 0x0600
    return VirtualAllocExNuma(
        GetCurrentProcess(),
        NULL,
        size,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE,
        node
    );
    #else
    return NULL;
    #endif

#elif THERON_GCC

    if ((numa_available() < 0))
    {
        return NULL;
    }

    return numa_alloc_onnode(size, node);

#endif

#endif // THERON_NUMA

    return NULL;
}

//----------------------------------------------------------------------
//-- free memory allocated by AllocOnNode -- needs mem and size
//----------------------------------------------------------------------
inline void FreeOnNode(void *mem, const size_t size)
{
#if THERON_NUMA

#if THERON_WINDOWS

    VirtualFree(mem, size, MEM_RELEASE);

#elif THERON_GCC

    numa_free(mem, size);

#endif

#endif // THERON_NUMA
}

#endif /*INCLUDED_LIBGRAS_ALLOC_ON_NODE_HPP*/
