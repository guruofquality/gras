// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/buffer_queue.hpp>
#include <gras_impl/debug.hpp>
#include <boost/format.hpp>
#include <boost/bind.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/anonymous_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <ctime>

using namespace gras;
namespace ipc = boost::interprocess;

static boost::mutex alloc_mutex;

/*!
* This routine generates an incredibly unique name for the allocation.
*
* Because we are using boost IPC, and it expects that we share the memory;
* IPC allocation requires a unique name to share amongst the processes.
* Since we are not actually using IPC, the sharing aspect isnt very useful,
* but we still need a unique name for the shared memory allocation anyway.
* (I would like if a empty string would suffice as an anonymous allocation)
*/
static std::string omg_so_unique(void)
{
    const std::string tid = boost::lexical_cast<std::string>(boost::this_thread::get_id());
    static size_t count = 0;
    return boost::str(boost::format("shmem-%s-%u-%u") % tid % count++ % clock());
}

//! Round a number (length or address) to an IPC boundary
static size_t round_up_to_ipc_page(const size_t bytes)
{
    const size_t chunk = ipc::mapped_region::get_page_size();
    return chunk*((bytes + chunk - 1)/chunk);
}

/*!
 * OK, we need a chunk of virtual memory that can be mapped.
 * But we cant actually be holding exclusive access to that memory
 * until it it mapped... This is naturally a race condition.
 */
static void *probably_get_vmem(const size_t length)
{
    void *addr = NULL;

    /*
    #if defined(linux) || defined(__linux) || defined(__linux__)
    addr = mmap(NULL, length, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    munmap(addr, length);
    return addr;
    #endif
    //*/

    const std::string shm_name = omg_so_unique();

    //std::cout << "make shmem 2x\n" << std::endl;
    ipc::shared_memory_object shm_obj_2x(
        ipc::create_only, //only create
        shm_name.c_str(), //name
        ipc::read_write //read-write mode
    );

    //std::cout << "truncate 2x\n" << std::endl;
    shm_obj_2x.truncate(length);

    {
        //std::cout << "map region 0\n" << std::endl;
        ipc::mapped_region region0(
            shm_obj_2x, //Memory-mappable object
            ipc::read_write, //Access mode
            0, //Offset from the beginning of shm
            length //Length of the region
        );
        //std::cout << "region0.get_address() " << size_t(region0.get_address()) << std::endl;
        addr = region0.get_address();
    }

    ipc::shared_memory_object::remove(shm_name.c_str());
    return addr;
}

struct CircularBuffer
{
    CircularBuffer(const size_t);

    ~CircularBuffer(void)
    {
        ipc::shared_memory_object::remove(shm_name.c_str());
    }

    char *buff_addr;
    const size_t actual_length;
    std::string shm_name;
    ipc::shared_memory_object shm_obj;
    ipc::mapped_region region1;
    ipc::mapped_region region2;
};

CircularBuffer::CircularBuffer(const size_t num_bytes):
    buff_addr(NULL),
    actual_length(round_up_to_ipc_page(num_bytes))
{
    ////////////////////////////////////////////////////////////////
    // Step 0) Find an address that can be mapped across 2x length:
    ////////////////////////////////////////////////////////////////
    buff_addr = (char *)probably_get_vmem(actual_length*2);
    //std::cout << "reserve addr " << std::hex << size_t(buff_addr) << std::dec << std::endl;

    ////////////////////////////////////////////////////////////////
    // Step 1) Allocate a chunk of physical memory of length bytes
    ////////////////////////////////////////////////////////////////
    //std::cout << "make shmem\n" << std::endl;
    shm_name = omg_so_unique();
    shm_obj = ipc::shared_memory_object(
        ipc::create_only, //only create
        shm_name.c_str(), //name
        ipc::read_write //read-write mode
    );

    //std::cout << "truncate\n" << std::endl;
    shm_obj.truncate(actual_length);

    ////////////////////////////////////////////////////////////////
    //Step 2) Remap region1 of the virtual memory space
    ////////////////////////////////////////////////////////////////
    //std::cout << "map region 1\n" << std::endl;
    region1 = ipc::mapped_region(
        shm_obj, //Memory-mappable object
        ipc::read_write, //Access mode
        0, //Offset from the beginning of shm
        actual_length, //Length of the region
        buff_addr
    );
    //std::cout << "region1.get_address() " << size_t(region1.get_address()) << std::endl;

    ////////////////////////////////////////////////////////////////
    //Step 3) Remap region2 of the virtual memory space
    ////////////////////////////////////////////////////////////////
    //std::cout << "map region 2\n" << std::endl;
    region2 = ipc::mapped_region(
        shm_obj, //Memory-mappable object
        ipc::read_write, //Access mode
        0, //Offset from the beginning of shm
        actual_length, //Length of the region
        buff_addr + actual_length
    );

    //std::cout << "region2.get_address() " << size_t(region2.get_address()) << std::endl;
    //std::cout << "diff " << (long(region2.get_address()) - long(region1.get_address())) << std::endl;

    ////////////////////////////////////////////////////////////////
    //4) Self memory test
    ////////////////////////////////////////////////////////////////
    boost::uint32_t *mem = (boost::uint32_t*)buff_addr;
    for (size_t i = 0; i < actual_length/sizeof(*mem); i++)
    {
        const boost::uint32_t num = std::rand();
        mem[i] = num;
        ASSERT(mem[i+actual_length/sizeof(*mem)] == num);
    }

    ////////////////////////////////////////////////////////////////
    //5) Zero out the memory for good measure
    ////////////////////////////////////////////////////////////////
    std::memset(buff_addr, 0, actual_length);
}

static void circular_buffer_delete(SBuffer &buff, CircularBuffer *circ_buff)
{
    boost::mutex::scoped_lock lock(alloc_mutex);
    delete circ_buff;
}

SBuffer make_circular_buffer(const size_t num_bytes)
{
    boost::mutex::scoped_lock lock(alloc_mutex);
    CircularBuffer *circ_buff = NULL;
    size_t trial_count = 0;
    while (circ_buff == NULL)
    {
        trial_count++;
        try
        {
            circ_buff = new CircularBuffer(num_bytes);
        }
        catch(const boost::interprocess::interprocess_exception &ex)
        {
            std::cerr << boost::format(
                "GRAS: make_circular_buffer threw ipc exception on attempt %u\n%s"
            ) % trial_count % ex.what() << std::endl;
            if (trial_count == 3) throw ex;
        }
        catch(...)
        {
            throw;
        }
    }
    SBufferDeleter deleter = boost::bind(&circular_buffer_delete, _1, circ_buff);
    SBufferConfig config;
    config.memory = circ_buff->buff_addr;
    config.length = circ_buff->actual_length;
    config.deleter = deleter;
    return SBuffer(config);
}
