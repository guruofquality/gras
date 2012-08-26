########################################################################
# This file included, use CMake directory variables
########################################################################

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../tests)

########################################################################
# Append gnuradio-core library sources
########################################################################
list(APPEND gnuradio_core_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sys_paths.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_message.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_msg_queue.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_msg_handler.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/element.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block_task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block_handlers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hier_block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/top_block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sync_block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_hier_block2.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_top_block.cpp
)

########################################################################
# Append gnuradio-core test sources
########################################################################
list(APPEND test_gnuradio_core_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/../tests/qa_gr_block.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/../tests/qa_gr_hier_block2.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/../tests/qa_gr_hier_block2_derived.cc
    #${CMAKE_CURRENT_SOURCE_DIR}/../tests/qa_gr_buffer.cc
    #${CMAKE_CURRENT_SOURCE_DIR}/../tests/qa_gr_flowgraph.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/../tests/qa_gr_top_block.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/../tests/qa_gr_io_signature.cc
    #${CMAKE_CURRENT_SOURCE_DIR}/../tests/qa_gr_vmcircbuf.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/../tests/qa_block_tags.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/../tests/qa_runtime.cc
    #${CMAKE_CURRENT_SOURCE_DIR}/../tests/qa_set_msg_handler.cc
)

########################################################################
# Find TSBE and add to libs
########################################################################
FIND_PATH(
    TSBE_INCLUDE_DIRS
    NAMES tsbe/config.hpp
    PATHS /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    TSBE_LIBRARIES
    NAMES tsbe
    PATHS /usr/local/lib
          /usr/lib
          /usr/lib64
)

include_directories(${TSBE_INCLUDE_DIRS})
link_directories(${TSBE_LIBRARY_DIRS})

list(APPEND gnuradio_core_libs
    ${TSBE_LIBRARIES}
)

if(LINUX)
    list(APPEND gnuradio_core_libs numa)
endif()
