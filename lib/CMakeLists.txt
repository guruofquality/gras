########################################################################
# This file included, use CMake directory variables
########################################################################
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(GRAS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/../)
set(GRAS_BINARY_DIR ${CMAKE_BINARY_DIR}/gruel/src)
set(RUNTIME_SOURCE_DIR ${CMAKE_SOURCE_DIR}/gnuradio-core/src/lib/runtime)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DGRAS_DEBUG)
endif()

########################################################################
# Setup Theron Deps
########################################################################
set(THERON_SOURCE_DIR ${GRAS_SOURCE_DIR}/Theron)
include(${THERON_SOURCE_DIR}/TheronSetup.cmake)
include_directories(${THERON_INCLUDE_DIRS})
link_directories(${THERON_LIBRARY_DIRS})
add_definitions(${THERON_DEFINES})

list(APPEND gnuradio_core_libs ${THERON_LIBRARIES})
list(APPEND gnuradio_core_sources ${THERON_SOURCES})

########################################################################
# Setup Apology Deps
########################################################################
include_directories(${GRAS_SOURCE_DIR}/Apology/include)
include_directories(${GRAS_SOURCE_DIR}/Apology/lib)

file(GLOB apology_sources "${GRAS_SOURCE_DIR}/Apology/lib/*.cpp")
list(APPEND gnuradio_core_sources ${apology_sources})

########################################################################
# Setup NumaNuma
########################################################################
list(APPEND CMAKE_MODULE_PATH ${GRAS_SOURCE_DIR}/numanuma/cmake)
include_directories(${GRAS_SOURCE_DIR}/numanuma/include)
find_package(NumaNuma)
if(NOT NUMANUMA_FOUND)
    message(FATAL_ERROR "numanuma dependencies not met!")
endif()

list(APPEND gnuradio_core_libs ${NUMANUMA_LIBRARIES})

########################################################################
# Append gnuradio-core library sources
########################################################################
list(APPEND gnuradio_core_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/debug.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/element.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sbuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block_actor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block_task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block_allocator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block_handlers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/topology_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/input_handlers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/output_handlers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hier_block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/top_block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sync_block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_hier_block2.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_top_block.cpp
)

#sources that are in tree that have not changed
list(APPEND gnuradio_core_sources
    ${RUNTIME_SOURCE_DIR}/gr_sys_paths.cc
    ${RUNTIME_SOURCE_DIR}/gr_message.cc
    ${RUNTIME_SOURCE_DIR}/gr_msg_queue.cc
    ${RUNTIME_SOURCE_DIR}/gr_msg_handler.cc
)

########################################################################
# Append gnuradio-core test sources
########################################################################
list(APPEND test_gnuradio_core_sources
    ${RUNTIME_SOURCE_DIR}/qa_gr_block.cc
    ${RUNTIME_SOURCE_DIR}/qa_gr_hier_block2.cc
    ${RUNTIME_SOURCE_DIR}/qa_gr_hier_block2_derived.cc
    #${RUNTIME_SOURCE_DIR}/qa_gr_buffer.cc
    #${RUNTIME_SOURCE_DIR}/qa_gr_flowgraph.cc
    ${RUNTIME_SOURCE_DIR}/qa_gr_top_block.cc
    ${RUNTIME_SOURCE_DIR}/qa_gr_io_signature.cc
    #${RUNTIME_SOURCE_DIR}/qa_gr_vmcircbuf.cc
    ${RUNTIME_SOURCE_DIR}/qa_block_tags.cc
    ${GRAS_SOURCE_DIR}/lib/qa_runtime.cc
    #${RUNTIME_SOURCE_DIR}/qa_set_msg_handler.cc
)

#copy test headers to include dir
set(test_headers
    ${RUNTIME_SOURCE_DIR}/qa_gr_block.h
    ${RUNTIME_SOURCE_DIR}/qa_gr_hier_block2.h
    ${RUNTIME_SOURCE_DIR}/qa_gr_hier_block2_derived.h
    #${RUNTIME_SOURCE_DIR}/qa_gr_buffer.h
    #${RUNTIME_SOURCE_DIR}/qa_gr_flowgraph.h
    ${RUNTIME_SOURCE_DIR}/qa_gr_top_block.h
    ${RUNTIME_SOURCE_DIR}/qa_gr_io_signature.h
    #${RUNTIME_SOURCE_DIR}/qa_gr_vmcircbuf.h
    ${RUNTIME_SOURCE_DIR}/qa_block_tags.h
    ${RUNTIME_SOURCE_DIR}/qa_runtime.h
    #${RUNTIME_SOURCE_DIR}/qa_set_msg_handler.h
    ${RUNTIME_SOURCE_DIR}/gr_unittests.h
)
foreach(header ${test_headers})
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${header} ${GRAS_BINARY_DIR}/include)
endforeach(header)

########################################################################
# Install runtime headers
########################################################################
set(runtime_copy_headers
    ${RUNTIME_SOURCE_DIR}/gr_sys_paths.h
    ${RUNTIME_SOURCE_DIR}/gr_message.h
    ${RUNTIME_SOURCE_DIR}/gr_msg_queue.h
    ${RUNTIME_SOURCE_DIR}/gr_msg_handler.h
    ${RUNTIME_SOURCE_DIR}/gr_complex.h
)

#copy the headers to a place that is in the include path
foreach(runtime_copy_header ${runtime_copy_headers})
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${runtime_copy_header} ${GRAS_BINARY_DIR}/include)
endforeach(runtime_copy_header)

file(GLOB runtime_headers "${GRAS_SOURCE_DIR}/include/gnuradio/*")
install(FILES
    ${runtime_headers} ${runtime_copy_headers}
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
    COMPONENT "core_devel"
)

########################################################################
# Install swig headers
########################################################################
if(ENABLE_PYTHON)

set(runtime_copy_swigs
    ${RUNTIME_SOURCE_DIR}/gr_message.i
    ${RUNTIME_SOURCE_DIR}/gr_msg_queue.i
    ${RUNTIME_SOURCE_DIR}/gr_msg_handler.i
)

#makes swig doc generator happy
execute_process(
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/gnuradio-core/src/lib/runtime/
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/gnuradio-core/src/lib/nop.h
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/gnuradio-core/src/lib/nop.h
        ${CMAKE_BINARY_DIR}/gnuradio-core/src/lib/runtime/nop.h
)

#copy the headers to a place that is in the include path
foreach(runtime_copy_header ${runtime_copy_swigs})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/gnuradio-core/src/lib/swig/
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${runtime_copy_header} ${CMAKE_BINARY_DIR}/gnuradio-core/src/lib/swig/
    )
endforeach(runtime_copy_header)

file(GLOB runtime_swigs "${GRAS_SOURCE_DIR}/swig/*.i")
install(FILES
    ${runtime_swigs} ${runtime_copy_swigs}
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "core_swig"
)

endif(ENABLE_PYTHON)
