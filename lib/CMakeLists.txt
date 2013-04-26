########################################################################
# Setup the GRAS library build
########################################################################
include_directories(${GRAS_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

########################################################################
# Setup Boost
########################################################################
find_package(Boost COMPONENTS thread date_time regex system)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND GRAS_LIBRARIES ${Boost_LIBRARIES})

########################################################################
# Setup Theron Deps
########################################################################
set(THERON_SOURCE_DIR ${GRAS_SOURCE_DIR}/Theron)
include(${THERON_SOURCE_DIR}/TheronSetup.cmake)
include_directories(${THERON_INCLUDE_DIRS})
link_directories(${THERON_LIBRARY_DIRS})
add_definitions(${THERON_DEFINES})
add_definitions(-DTHERON_ENABLE_DEFAULTALLOCATOR_CHECKS=1)

list(APPEND GRAS_LIBRARIES ${THERON_LIBRARIES})
list(APPEND GRAS_SOURCES ${THERON_SOURCES})

########################################################################
# Setup Apology Deps
########################################################################
include_directories(${GRAS_SOURCE_DIR}/Apology/include)
include_directories(${GRAS_SOURCE_DIR}/Apology/lib)

file(GLOB apology_sources "${GRAS_SOURCE_DIR}/Apology/lib/*.cpp")
list(APPEND GRAS_SOURCES ${apology_sources})

########################################################################
# Append gras-core library sources
########################################################################
list(APPEND GRAS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/debug.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/element.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sbuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/circular_buffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/buffer_queue_circ.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/buffer_queue_pool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tags.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block_message.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block_consume.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block_produce.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block_props.cpp
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
    ${CMAKE_CURRENT_SOURCE_DIR}/top_block_query.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/register_messages.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/theron_allocator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/weak_container.cpp
)

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
