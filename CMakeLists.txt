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

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-Wall)
    add_definitions(-fvisibility=hidden)
    add_definitions(-fvisibility-inlines-hidden)
endif()

########################################################################
# Component names for install rules
########################################################################
if (NOT DEFINED GRAS_COMP_DEVEL)
    set(GRAS_COMP_DEVEL "gras_devel")
endif()
if (NOT DEFINED GRAS_COMP_RUNTIME)
    set(GRAS_COMP_RUNTIME "gras_runtime")
endif()
if (NOT DEFINED GRAS_COMP_PYTHON)
    set(GRAS_COMP_PYTHON "gras_python")
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
# Paths for python
########################################################################
list(APPEND GRAS_PYTHON_DIRS ${GRAS_SOURCE_DIR}/python)
list(APPEND GRAS_PYTHON_DIRS ${GRAS_BINARY_DIR}/python)
list(APPEND GRAS_PYTHON_DIRS ${GRAS_BINARY_DIR}/python/gras)
list(APPEND GRAS_PYTHON_DIRS ${GRAS_BINARY_DIR}/python/gras/${CMAKE_BUILD_TYPE})

list(APPEND GRAS_PYTHON_DIRS ${GRAS_SOURCE_DIR}/PMC/python)
list(APPEND GRAS_PYTHON_DIRS ${GRAS_BINARY_DIR}/PMC/python)
list(APPEND GRAS_PYTHON_DIRS ${GRAS_BINARY_DIR}/PMC/python/PMC)
list(APPEND GRAS_PYTHON_DIRS ${GRAS_BINARY_DIR}/PMC/python/PMC/${CMAKE_BUILD_TYPE})

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gras)
add_subdirectory(lib)
add_subdirectory(PMC)
add_subdirectory(python/gras)
add_subdirectory(tests)

########################################################################
# add gnuradio as sub-project
########################################################################
set(CMAKE_SOURCE_DIR ${GRAS_SOURCE_DIR}/gnuradio)
set(CMAKE_BINARY_DIR ${GRAS_BINARY_DIR}/gnuradio)

set(GR_TEST_EXTRA_PYTHON_DIRS ${GRAS_PYTHON_DIRS}) #helps QA tests

set(GRAS_LIBRARIES gras) #for submodule linking

set(CMAKE_PROJECT_NAME gnuradio) #for submodule vars

add_subdirectory(gnuradio)

########################################################################
# add GRAS to gnuradio cpack registry
# this must come after the submodule
########################################################################
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
include(GrPackage)

CPACK_SET(CPACK_COMPONENT_GROUP_GRAS_DESCRIPTION "GNU Radio Advanced Scheduler")

CPACK_COMPONENT("${GRAS_COMP_RUNTIME}"
    GROUP        "GRAS"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Runtime"
)

CPACK_COMPONENT("${GRAS_COMP_DEVEL}"
    GROUP        "GRAS"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers and import libraries"
)

CPACK_COMPONENT("${GRAS_COMP_PYTHON}"
    GROUP        "GRAS"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime"
    DEPENDS      "${GRAS_COMP_RUNTIME}"
)
