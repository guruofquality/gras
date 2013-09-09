if(DEFINED __INCLUDED_GRAS_TOOL_CMAKE)
    return()
endif()
set(__INCLUDED_GRAS_TOOL_CMAKE TRUE)

include(GRASCommon)

#reset global variable on include
set(GRAS_TOOL_MODULE_LOCATIONS "" CACHE INTERNAL "")

########################################################################
## Set installation constants
## The GRAS_ROOT can be set via arg or environment variable
########################################################################
if(GRAS_ROOT)
    #its already set
elseif($ENV{GRAS_ROOT})
    set(GRAS_ROOT $ENV{GRAS_ROOT})
else()
    set(GRAS_ROOT "@CMAKE_INSTALL_PREFIX@")
endif()

########################################################################
## where to look for development files
########################################################################
set(GRAS_TOOL_INCLUDE_DIR ${GRAS_ROOT}/include)
set(GRAS_TOOL_LIBRARY_DIR ${GRAS_ROOT}/lib@LIBSUFFIX@)

#locate PMC and GRAS includes
if (NOT PMC_INCLUDE_DIRS)
    find_path(
        PMC_INCLUDE_DIRS
        NAMES PMC/PMC.hpp
        PATHS ${GRAS_TOOL_INCLUDE_DIR}
    )
endif()
if (NOT GRAS_INCLUDE_DIRS)
    find_path(
        GRAS_INCLUDE_DIRS
        NAMES gras/gras.hpp
        PATHS ${GRAS_TOOL_INCLUDE_DIR}
    )
endif()

#locate PMC and GRAS libraries
if (NOT PMC_LIBRARIES)
    find_library(
        PMC_LIBRARIES
        NAMES pmc
        PATHS ${GRAS_TOOL_LIBRARY_DIR}
    )
endif()
if (NOT GRAS_LIBRARIES)
    find_library(
        GRAS_LIBRARIES
        NAMES gras
        PATHS ${GRAS_TOOL_LIBRARY_DIR}
    )
endif()

########################################################################
## GRAS_TOOL cmake function - the swiss army knife for GRAS users
##
## Options:
## SOURCES - list of C++, Python, and GRC sources
## TARGET - project name, used for library, and install prefix
## DIRECTORY - name of installation directory or ${TARGET}
## COMPONENT - name of installation component or ${TARGET}
##
## External vars:
## GRAS_TOOL_INCLUDES - list of additional include directories
## GRAS_TOOL_LIBRARIES - list of additional libraries to link to
########################################################################
function(GRAS_TOOL)

    include(CMakeParseArguments)
    CMAKE_PARSE_ARGUMENTS(GRAS_TOOL "" "TARGET;DIRECTORY;COMPONENT" "SOURCES" ${ARGN})

    #give an install directory if not specified
    if(NOT GRAS_TOOL_DIRECTORY)
        set(GRAS_TOOL_DIRECTORY ${GRAS_TOOL_TARGET})
    endif()

    #give the target a component name if not specified
    if(NOT GRAS_TOOL_COMPONENT)
        set(GRAS_TOOL_COMPONENT ${GRAS_TOOL_TARGET})
    endif()

    unset(GRAS_TOOL_CPP_SOURCES)
    unset(GRAS_TOOL_PY_SOURCES)
    unset(GRAS_TOOL_GRC_SOURCES)
    foreach(source ${GRAS_TOOL_SOURCES})
        get_filename_component(source_ext ${source} EXT)
        if ("${source_ext}" STREQUAL ".cpp")
            list(APPEND GRAS_TOOL_CPP_SOURCES ${source})
        endif()
        if ("${source_ext}" STREQUAL ".cxx")
            list(APPEND GRAS_TOOL_CPP_SOURCES ${source})
        endif()
        if ("${source_ext}" STREQUAL ".cc")
            list(APPEND GRAS_TOOL_CPP_SOURCES ${source})
        endif()
        if ("${source_ext}" STREQUAL ".c")
            list(APPEND GRAS_TOOL_CPP_SOURCES ${source})
        endif()
        if ("${source_ext}" STREQUAL ".py")
            list(APPEND GRAS_TOOL_PY_SOURCES ${source})
        endif()
        if ("${source_ext}" STREQUAL ".xml")
            list(APPEND GRAS_TOOL_GRC_SOURCES ${source})
        endif()
        if ("${source_ext}" STREQUAL ".yml")
            list(APPEND GRAS_TOOL_GRC_SOURCES ${source})
        endif()
    endforeach(source)

    #suffix install path for project name
    set(GRAS_TOOL_MOD_DIR lib@LIBSUFFIX@/gras/modules/${GRAS_TOOL_DIRECTORY})
    set(GRAS_TOOL_PYTHON_DIR lib@LIBSUFFIX@/gras/python/${GRAS_TOOL_DIRECTORY})
    set(GRAS_TOOL_GRC_DIR share/gnuradio/grc/blocks/${GRAS_TOOL_DIRECTORY})

    #development and framework directories
    list(APPEND GRAS_TOOL_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR})
    list(APPEND GRAS_TOOL_INCLUDES ${PMC_INCLUDE_DIRS})
    list(APPEND GRAS_TOOL_INCLUDES ${GRAS_INCLUDE_DIRS})
    list(APPEND GRAS_TOOL_LIBRARIES ${PMC_LIBRARIES})
    list(APPEND GRAS_TOOL_LIBRARIES ${GRAS_LIBRARIES})

    #and boost includes as well
    include_directories(${Boost_INCLUDE_DIRS})

    #required directories for compiling
    include_directories(${GRAS_TOOL_INCLUDES})

    #build and install module to path
    if (GRAS_TOOL_CPP_SOURCES)
        add_library(${GRAS_TOOL_TARGET} MODULE ${GRAS_TOOL_CPP_SOURCES})
        target_link_libraries(${GRAS_TOOL_TARGET} ${GRAS_TOOL_LIBRARIES})
        install(TARGETS ${GRAS_TOOL_TARGET}
            LIBRARY DESTINATION ${GRAS_TOOL_MOD_DIR} COMPONENT ${GRAS_TOOL_COMPONENT} # .so file
            ARCHIVE DESTINATION ${GRAS_TOOL_MOD_DIR} COMPONENT ${GRAS_TOOL_COMPONENT} # .lib file
            RUNTIME DESTINATION ${GRAS_TOOL_MOD_DIR} COMPONENT ${GRAS_TOOL_COMPONENT} # .dll file
        )

        #export global variables for help locating build targets
        get_target_property(module_location ${GRAS_TOOL_TARGET} LOCATION)
        string(REGEX REPLACE "\\$\\(.*\\)" ${CMAKE_BUILD_TYPE} module_location ${module_location})
        set(GRAS_TOOL_MODULE_LOCATIONS "${GRAS_TOOL_MODULE_LOCATIONS};${module_location}" CACHE INTERNAL "")
    endif()

    #python module install
    if (GRAS_TOOL_PY_SOURCES)
        install(
            FILES ${GRAS_TOOL_PY_SOURCES}
            DESTINATION ${GRAS_TOOL_PYTHON_DIR}
            COMPONENT ${GRAS_TOOL_COMPONENT}
        )
    endif()

    #install GRC files
    if (GRAS_TOOL_GRC_SOURCES)
        install(
            FILES ${GRAS_TOOL_GRC_SOURCES}
            DESTINATION ${GRAS_TOOL_GRC_DIR}
            COMPONENT ${GRAS_TOOL_COMPONENT}
        )
    endif()

endfunction(GRAS_TOOL)
