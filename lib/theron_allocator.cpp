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
#define MY_ALLOCATOR_POOL_SIZE (MY_ALLOCATOR_CHUNK_SIZE * (1 << 16))

static struct WorkerAllocator : Theron::IAllocator
{
    WorkerAllocator(void)
    {
        const size_t N = MY_ALLOCATOR_POOL_SIZE/MY_ALLOCATOR_CHUNK_SIZE;
        queue.resize(N);
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
        if (size <= MY_ALLOCATOR_CHUNK_SIZE)
        {
            mSpinLock.Lock();
            if (queue.empty())
            {
                mSpinLock.Unlock();
                std::cout << "~" << std::flush;
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
            return new char[size];
        }
    }

    void Free(void *const memory)
    {
        const bool in_pool = ptrdiff_t(memory) >= ptrdiff_t(pool) and ptrdiff_t(memory) < pool_end;
        if (in_pool)
        {
            mSpinLock.Lock();
            queue.push_front(memory);
            mSpinLock.Unlock();
        }
        else
        {
            delete [] ((char *)memory);
        }
    }

    boost::circular_buffer<void *> queue;
    THERON_PREALIGN(GRAS_MAX_ALIGNMENT)
        char pool[MY_ALLOCATOR_POOL_SIZE]
    THERON_POSTALIGN(GRAS_MAX_ALIGNMENT);
    ptrdiff_t pool_end;
    Theron::Detail::SpinLock mSpinLock;

} my_alloc;
