########################################################################
# Setup the GRAS library build
########################################################################
include_directories(${GRAS_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

########################################################################
# Setup Boost
########################################################################
find_package(Boost COMPONENTS thread date_time regex system filesystem)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND GRAS_LIBRARIES ${Boost_LIBRARIES})

########################################################################
# Setup Theron Deps
########################################################################
set(THERON_SOURCE_DIR ${GRAS_SOURCE_DIR}/Theron)
include(${THERON_SOURCE_DIR}/cmake/TheronSetup.cmake)
include_directories(${THERON_INCLUDE_DIRS})
link_directories(${THERON_LIBRARY_DIRS})
add_definitions(${THERON_DEFINES})

list(APPEND GRAS_LIBRARIES ${THERON_LIBRARIES})
list(APPEND GRAS_SOURCES ${THERON_SOURCES})

add_definitions(-DTHERON_ENABLE_DEFAULTALLOCATOR_CHECKS=1)
add_definitions(-DTHERON_ENABLE_COUNTERS=1)

########################################################################
# Setup Apology Deps
########################################################################
include_directories(${GRAS_SOURCE_DIR}/Apology/include)
include_directories(${GRAS_SOURCE_DIR}/Apology/lib)

file(GLOB apology_sources "${GRAS_SOURCE_DIR}/Apology/lib/*.cpp")
list(APPEND GRAS_SOURCES ${apology_sources})

########################################################################
# Setup Module Loader
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/module_loader.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/module_loader.cpp
@ONLY)
list(APPEND GRAS_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/module_loader.cpp
)
list(APPEND GRAS_LIBRARIES ${CMAKE_DL_LIBS})

########################################################################
# Append gras-core library sources
########################################################################
list(APPEND GRAS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/debug.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/callable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/element.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/element_uid.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/factory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/jit_factory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sbuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/circular_buffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/buffer_queue_circ.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/buffer_queue_pool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tags.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/time_tag.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block_message.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block_consume.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block_produce.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block_calls.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thread_pool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block_actor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/task_done.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/task_fail.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/task_main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block_allocator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block_handlers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/topology_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/input_handlers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/output_handlers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hier_block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/top_block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/register_messages.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/weak_container.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/serialize_types.cpp
)

if (${Boost_VERSION} LESS 104100)

    message(WARNING "Boost library version < 104100 does not support property tree - GRAS will be compiled without query interface")
    list(APPEND GRAS_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/top_block_query_no_ptree.cpp
    )

else()

    list(APPEND GRAS_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/top_block_query.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/json_parser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/pmc_to_ptree.cpp
    )

endif()

########################################################################
# Setup factory compiler
########################################################################

#step 1) call into the LLVM find package
find_program(LLVM_CONFIG_EXECUTABLE llvm-config DOC "llvm-config executable")
if(LLVM_CONFIG_EXECUTABLE)
    find_package(LLVM)
endif()

#step 2) setup LLVM if found and find Clang
if(LLVM_FOUND)
    set(HAVE_JIT_FACTORY TRUE PARENT_SCOPE)
    add_definitions(-DHAVE_LLVM)
    add_definitions(${LLVM_CFLAGS})
    link_directories(${LLVM_LIBRARY_DIRS})
    include_directories(${LLVM_INCLUDE_DIRS})
    list(APPEND GRAS_LIBRARIES ${LLVM_MODULE_LIBS})
    list(APPEND GRAS_LIBRARIES ${LLVM_LDFLAGS})
    find_package(Clang)
endif()

#step 3) setup Clang if found
if(CLANG_FOUND)
    add_definitions(-DHAVE_CLANG)
    include_directories(${CLANG_INCLUDE_DIRS})
    list(APPEND GRAS_LIBRARIES ${CLANG_LIBS})
endif()

########################################################################
# Build library
########################################################################
add_library(gras SHARED ${GRAS_SOURCES})
target_link_libraries(gras ${GRAS_LIBRARIES})
set_target_properties(gras PROPERTIES VERSION ${GRAS_VERSION})
set_target_properties(gras PROPERTIES DEFINE_SYMBOL "GRAS_DLL_EXPORTS")

install(TARGETS gras
    LIBRARY DESTINATION lib${LIB_SUFFIX} COMPONENT ${GRAS_COMP_RUNTIME} # .so file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT ${GRAS_COMP_DEVEL}   # .lib file
    RUNTIME DESTINATION bin              COMPONENT ${GRAS_COMP_RUNTIME} # .dll file
)

########################################################################
# Build pkg config file
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gras.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gras.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gras.pc
    DESTINATION lib${LIB_SUFFIX}/pkgconfig
    COMPONENT ${GRAS_COMP_DEVEL}
)
