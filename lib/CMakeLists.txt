########################################################################
# This file included, use CMake directory variables
########################################################################

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${GRAS_SOURCE_DIR}/tests)

set(GRAS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/../)

########################################################################
# Setup NumaNuma
########################################################################
list(APPEND CMAKE_MODULE_PATH ${GRAS_SOURCE_DIR}/numanuma/cmake)
include_directories(${GRAS_SOURCE_DIR}/numanuma/include)
find_package(NumaNuma)
if(NOT NUMANUMA_FOUND)
    message(FATAL_ERROR "numanuma dependencies not met!")
endif()

########################################################################
# Append gnuradio-core library sources
########################################################################
list(APPEND gnuradio_core_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/element.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block_task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block_allocator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block_handlers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/port_handlers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hier_block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/top_block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sync_block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_hier_block2.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_top_block.cpp
)

#sources that are in tree that have not changed
set(runtime_srcs_dir ${CMAKE_SOURCE_DIR}/gnuradio-core/src/lib/runtime)
list(APPEND gnuradio_core_sources
    ${runtime_srcs_dir}/gr_sys_paths.cc
    ${runtime_srcs_dir}/gr_message.cc
    ${runtime_srcs_dir}/gr_msg_queue.cc
    ${runtime_srcs_dir}/gr_msg_handler.cc
)

########################################################################
# Append gnuradio-core test sources
########################################################################
list(APPEND test_gnuradio_core_sources
    ${GRAS_SOURCE_DIR}/tests/qa_gr_block.cc
    ${GRAS_SOURCE_DIR}/tests/qa_gr_hier_block2.cc
    ${GRAS_SOURCE_DIR}/tests/qa_gr_hier_block2_derived.cc
    #${GRAS_SOURCE_DIR}/tests/qa_gr_buffer.cc
    #${GRAS_SOURCE_DIR}/tests/qa_gr_flowgraph.cc
    ${GRAS_SOURCE_DIR}/tests/qa_gr_top_block.cc
    ${GRAS_SOURCE_DIR}/tests/qa_gr_io_signature.cc
    #${GRAS_SOURCE_DIR}/tests/qa_gr_vmcircbuf.cc
    ${GRAS_SOURCE_DIR}/tests/qa_block_tags.cc
    ${GRAS_SOURCE_DIR}/tests/qa_runtime.cc
    #${GRAS_SOURCE_DIR}/tests/qa_set_msg_handler.cc
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

list(APPEND gnuradio_core_libs ${NUMANUMA_LIBRARIES})

########################################################################
# Install runtime headers
########################################################################
file(GLOB runtime_headers "${GRAS_SOURCE_DIR}/include/gnuradio/*")
install(FILES
    ${runtime_headers}
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
    COMPONENT "core_devel"
)

########################################################################
# Install swig headers
########################################################################
if(ENABLE_PYTHON)
file(GLOB runtime_swigs "${GRAS_SOURCE_DIR}/swig/*.i")
install(FILES
    ${runtime_swigs}
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "core_swig"
)

#makes swig doc generator happy
execute_process(
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/gnuradio-core/src/lib/runtime/
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/gnuradio-core/src/lib/runtime/nop.h
)

endif(ENABLE_PYTHON)
