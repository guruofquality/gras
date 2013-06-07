// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

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
        std::cerr << ">>> GRAS: The debug asserts are enabled. <<<" << std::endl;
    }
};

static const DebugAssertPrinter dap;

#endif
