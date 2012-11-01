########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 2.6)
project(gras CXX C)
enable_testing()

set(GRAS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(GRAS_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

list(APPEND CMAKE_MODULE_PATH ${GRAS_SOURCE_DIR}/PMC/cmake/Modules)
include(CMakeDefsHelper)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DGRAS_DEBUG)
endif()

set(LIBVER 0.0.0)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-Wall)
    add_definitions(-fvisibility=hidden)
    add_definitions(-fvisibility-inlines-hidden)
endif()

add_definitions(-DGRAS_DLL=1) #this project builds a dll

########################################################################
# Component names for install rules
########################################################################
if (NOT DEFINED GRAS_COMP_DEVEL)
    set(GRAS_COMP_DEVEL "devel")
endif()
if (NOT DEFINED GRAS_COMP_RUNTIME)
    set(GRAS_COMP_RUNTIME "runtime")
endif()
if (NOT DEFINED GRAS_COMP_PYTHON)
    set(GRAS_COMP_PYTHON "python")
endif()

set(PMC_COMP_DEVEL ${GRAS_COMP_DEVEL})
set(PMC_COMP_RUNTIME ${GRAS_COMP_RUNTIME})
set(PMC_COMP_PYTHON ${GRAS_COMP_PYTHON})

########################################################################
# Paths to public headers
########################################################################
list(APPEND GRAS_INCLUDE_DIRS ${GRAS_SOURCE_DIR}/PMC/include)
list(APPEND GRAS_INCLUDE_DIRS ${GRAS_SOURCE_DIR}/include)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gras)
add_subdirectory(lib)
add_subdirectory(PMC)
add_subdirectory(python/gras)
add_subdirectory(tests)
