########################################################################
# This file included, use CMake directory variables
########################################################################

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

########################################################################
# Append gnuradio-core library sources
########################################################################
list(APPEND gnuradio_core_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sys_paths.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_message.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_msg_queue.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_msg_handler.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/element.cpp
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
    NAMES tsbe_static
    PATHS /usr/local/lib
          /usr/lib
          /usr/lib64
)

include_directories(${TSBE_INCLUDE_DIRS})
link_directories(${TSBE_LIBRARY_DIRS})

list(APPEND gnuradio_core_libs
    ${TSBE_LIBRARIES}
)
