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

/***********************************************************************
 * There is allocation overhead for sending messages.
 * Want per worker-allocator for message allocation...
 * But until thats possible, install a new global allocator.
 * This allocator uses a fixed pool for small sized buffers,
 * and otherwise the regular malloc/free for larger buffers.
 **********************************************************************/

#include <gras_impl/debug.hpp>
#include <Theron/Detail/Threading/SpinLock.h>
#include <Theron/IAllocator.h>
#include <Theron/AllocatorManager.h>
#include <boost/circular_buffer.hpp>

#define MY_ALLOCATOR_CHUNK_SIZE 88 //Theron asks for a lot of 88-byte buffers
#define MY_ALLOCATOR_POOL_SIZE (MY_ALLOCATOR_CHUNK_SIZE * 4096)

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
            ASSERT(not queue.empty());
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
        const bool in_pool = ptrdiff_t(memory) >= ptrdiff_t(pool) and ptrdiff_t(memory) < (ptrdiff_t(pool) + MY_ALLOCATOR_POOL_SIZE);
        if (in_pool)
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
    long pool[MY_ALLOCATOR_POOL_SIZE/sizeof(long)];
    Theron::Detail::SpinLock mSpinLock;

} my_alloc;
