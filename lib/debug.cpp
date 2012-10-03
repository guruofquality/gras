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

#include <gras_impl/debug.hpp>

#ifdef MALLOC_DEBUG
#include <cstdio>
#include <cstdlib>

void *operator new(std::size_t n) throw (std::bad_alloc)
{
    //static int malloc_count = 0;
    //printf("malloc #%d -> size %u\n", malloc_count++, n);
    return std::malloc(n);
}
#endif

#ifdef ASSERTING

struct DebugAssertPrinter
{
    DebugAssertPrinter(void)
    {
        std::cerr
            << "###################################################\n"
            << "## ATTENTION!!! The debug asserts are enabled\n"
            << "###################################################\n"
            << std::flush;
    }
};

static const DebugAssertPrinter dap;

#endif
