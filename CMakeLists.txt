########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 2.6)
project(gras CXX C)
enable_testing()

#select the release build type by default to get optimization flags
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
   message(STATUS "Build type not specified: defaulting to release.")
endif(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DGRAS_DEBUG)
endif()

set(GRAS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

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

########################################################################
# Setup PMC Deps
########################################################################
set(PMC_COMP_DEVEL ${GRAS_COMP_DEVEL})
set(PMC_COMP_RUNTIME ${GRAS_COMP_RUNTIME})
set(PMC_COMP_PYTHON ${GRAS_COMP_PYTHON})

list(APPEND CMAKE_MODULE_PATH ${GRAS_SOURCE_DIR}/PMC/cmake/Modules)
include_directories(${GRAS_SOURCE_DIR}/PMC/include)
add_subdirectory(PMC)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(lib)
