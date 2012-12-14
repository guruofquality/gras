// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras_impl/endless_buffer_queue.hpp>
#include <boost/format.hpp>
#include <boost/bind.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <ctime>

using namespace gras;
namespace ipc = boost::interprocess;

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
    boost::uuids::uuid u1; // initialize uuid
    return boost::str(boost::format("shmem-%s-%u-%u-%u")
        % to_string(u1) % std::rand() % clock() % time(NULL));
}

struct CircularBuffer
{
    CircularBuffer(const size_t);

    ~CircularBuffer(void)
    {
        ipc::shared_memory_object::remove(shm_name.c_str());
    }

    char *buff_addr;
    size_t actual_length;
    std::string shm_name;
    ipc::shared_memory_object shm_obj;
    ipc::mapped_region region1;
    ipc::mapped_region region2;
};

CircularBuffer::CircularBuffer(const size_t num_bytes)
{
    const size_t chunk = ipc::mapped_region::get_page_size();
    const size_t len = chunk*((num_bytes + chunk - 1)/chunk);
    actual_length = len;

    ////////////////////////////////////////////////////////////////
    // Step 0) Find an address that can be mapped across 2x length:
    // Allocate physical memory 2x the required size.
    // Map a virtual memory region across this memory.
    // Now we have a 2x length swath of virtual memory.
    ////////////////////////////////////////////////////////////////
    {
        shm_name = omg_so_unique();

        //std::cout << "make shmem 2x\n" << std::endl;
        ipc::shared_memory_object shm_obj_2x(
            ipc::create_only, //only create
            shm_name.c_str(), //name
            ipc::read_write //read-write mode
        );

        //std::cout << "truncate 2x\n" << std::endl;
        shm_obj_2x.truncate(2*len);

        //std::cout << "map region 0\n" << std::endl;
        ipc::mapped_region region0(
            shm_obj_2x, //Memory-mappable object
            ipc::read_write, //Access mode
            0, //Offset from the beginning of shm
            2*len //Length of the region
        );
        //std::cout << "region0.get_address() " << size_t(region0.get_address()) << std::endl;

        ipc::shared_memory_object::remove(shm_name.c_str());
        buff_addr = (char *)region0.get_address();
    }

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
    shm_obj.truncate(len);

    ////////////////////////////////////////////////////////////////
    //Step 2) Remap region1 of the virtual memory space
    ////////////////////////////////////////////////////////////////
    //std::cout << "map region 1\n" << std::endl;
    region1 = ipc::mapped_region(
        shm_obj, //Memory-mappable object
        ipc::read_write, //Access mode
        0, //Offset from the beginning of shm
        len, //Length of the region
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
        len, //Length of the region
        buff_addr + len
    );

    //std::cout << "region2.get_address() " << size_t(region2.get_address()) << std::endl;
    //std::cout << "diff " << (long(region2.get_address()) - long(region1.get_address())) << std::endl;

    ////////////////////////////////////////////////////////////////
    //4) Zero out the memory for good measure
    ////////////////////////////////////////////////////////////////
    std::memset(region1.get_address(), 0, region1.get_size());
    std::memset(region2.get_address(), 0, region2.get_size());
}

static void circular_buffer_delete(SBuffer &buff, CircularBuffer *circ_buff)
{
    delete circ_buff;
}

SBuffer EndlessBufferQueue::make_circular_buffer(const size_t num_bytes)
{
    CircularBuffer *circ_buff = new CircularBuffer(num_bytes);
    SBufferDeleter deleter = boost::bind(&circular_buffer_delete, _1, circ_buff);
    SBufferConfig config;
    config.memory = circ_buff->buff_addr;
    config.length = circ_buff->actual_length;
    config.deleter = deleter;
    return SBuffer(config);
}
