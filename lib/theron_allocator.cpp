// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

/***********************************************************************
 * There is allocation overhead for sending messages.
 * Want per worker-allocator for message allocation...
 * But until thats possible, install a new global allocator.
 * This allocator uses a fixed pool for small sized buffers,
 * and otherwise the regular malloc/free for larger buffers.
 **********************************************************************/

#include <gras/gras.hpp>
#include <gras_impl/debug.hpp>
#include <Theron/Detail/Threading/SpinLock.h>
#include <Theron/IAllocator.h>
#include <Theron/AllocatorManager.h>
#include <boost/circular_buffer.hpp>

#define MY_ALLOCATOR_CHUNK_SIZE 256
#define MY_ALLOCATOR_POOL_SIZE (MY_ALLOCATOR_CHUNK_SIZE * (1 << 18))

static unsigned long long unwanted_malloc_count = 0;

static struct ExitPrinter
{
    ExitPrinter(void){}
    ~ExitPrinter(void)
    {
        if (unwanted_malloc_count)
        {
            VAR(unwanted_malloc_count);
        }
        VAR(((Theron::DefaultAllocator *)(Theron::AllocatorManager::Instance().GetAllocator()))->GetBytesAllocated());
        VAR(((Theron::DefaultAllocator *)(Theron::AllocatorManager::Instance().GetAllocator()))->GetPeakBytesAllocated());
        VAR(((Theron::DefaultAllocator *)(Theron::AllocatorManager::Instance().GetAllocator()))->GetAllocationCount());
    }
} exit_printer;



/*
static struct WorkerAllocator : Theron::IAllocator
{
    WorkerAllocator(void)
    {
        const size_t N = MY_ALLOCATOR_POOL_SIZE/MY_ALLOCATOR_CHUNK_SIZE;
        queue.set_capacity(N);
        for (size_t i = 0; i < N; i++)
        {
            const ptrdiff_t pool_ptr = ptrdiff_t(pool) + i*MY_ALLOCATOR_CHUNK_SIZE;
            queue.push_back((void *)pool_ptr);
        }
        pool_end = ptrdiff_t(pool) + MY_ALLOCATOR_POOL_SIZE;
        Theron::AllocatorManager::Instance().SetAllocator(this);
    }

    ~WorkerAllocator(void)
    {
        //NOP
    }

    void *Allocate(const SizeType size)
    {
        if GRAS_LIKELY(size <= MY_ALLOCATOR_CHUNK_SIZE)
        {
            mSpinLock.Lock();
            if GRAS_UNLIKELY(queue.empty())
            {
                unwanted_malloc_count++;
                mSpinLock.Unlock();
                return std::malloc(size);
            }
            void *memory = queue.front();
            queue.pop_front();
            mSpinLock.Unlock();
            return memory;
        }
        else
        {
            //std::cout << "malloc size " << size << std::endl;
            return std::malloc(size);
        }
    }

    void Free(void *const memory)
    {
        const bool in_pool = ptrdiff_t(memory) >= ptrdiff_t(pool) and ptrdiff_t(memory) < pool_end;
        if GRAS_LIKELY(in_pool)
        {
            mSpinLock.Lock();
            queue.push_front(memory);
            mSpinLock.Unlock();
        }
        else
        {
            std::free(memory);
        }
    }

    boost::circular_buffer<void *> queue;
    THERON_PREALIGN(GRAS_MAX_ALIGNMENT)
        char pool[MY_ALLOCATOR_POOL_SIZE]
    THERON_POSTALIGN(GRAS_MAX_ALIGNMENT);
    ptrdiff_t pool_end;
    Theron::Detail::SpinLock mSpinLock;

} my_alloc;
*/
