if(DEFINED __INCLUDED_GRAS_TOOL_CMAKE)
    return()
endif()
set(__INCLUDED_GRAS_TOOL_CMAKE TRUE)

########################################################################
## GRAS_TOOL_GET_PATH - query the GRAS tool for paths
## Query the options set by OPT
## Set the resulting path to VAR
########################################################################
function(GRAS_TOOL_GET_PATH VAR OPT)
    find_program(GRAS_TOOL_EXECUTABLE gras_tool)
    execute_process(
        COMMAND ${GRAS_TOOL_EXECUTABLE} --${OPT}
        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE GRAS_TOOL_VAR_OUT
    )
    message(STATUS "${OPT} = ${GRAS_TOOL_VAR_OUT}")
    set(${VAR} ${GRAS_TOOL_VAR_OUT} PARENT_SCOPE)
endfunction(GRAS_TOOL_GET_PATH)

########################################################################
## GRAS_TOOL cmake function - the swiss army knife for GRAS users
##
## Options:
## SOURCES - list of C++, Python, and GRC sources
## PROJECT - project name, used for library, and install prefix
## COMPONENT - optional component name for cmake INSTALL function
########################################################################
function(GRAS_TOOL)

    include(CMakeParseArguments)
    cmake_parse_arguments(GRAS_TOOL "" "PROJECT;COMPONENT" "SOURCES" ${ARGN})

    unset(GRAS_TOOL_CPP_SOURCES)
    unset(GRAS_TOOL_PY_SOURCES)
    unset(GRAS_TOOL_GRC_SOURCES)
    foreach(source ${GRAS_TOOL_SOURCES})
        get_filename_component(source_ext ${source} EXT)
        if ("${source_ext}" STREQUAL ".cpp")
            list(APPEND GRAS_TOOL_CPP_SOURCES ${source})
        endif()
        if ("${source_ext}" STREQUAL ".cc")
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

    #extract install directories
    GRAS_TOOL_GET_PATH(GRAS_ROOT "print-gras-root-dir")
    GRAS_TOOL_GET_PATH(GRAS_TOOL_MOD_DEST "print-library-mod-dir")
    GRAS_TOOL_GET_PATH(GRAS_TOOL_GRC_DEST "print-grc-blocks-dir")

    #locate PMC and GRAS includes
    find_path(
        GRAS_INCLUDE_DIRS
        NAMES gras/gras.hpp
        PATHS ${GRAS_ROOT}/include
    )
    include_directories(${GRAS_INCLUDE_DIRS})

    #locate PMC and GRAS libraries
    find_library(
        PMC_LIBRARIES
        NAMES pmc
        PATHS ${GRAS_ROOT}/lib ${GRAS_ROOT}/lib64
    )
    find_library(
        GRAS_LIBRARIES
        NAMES gras
        PATHS ${GRAS_ROOT}/lib ${GRAS_ROOT}/lib64
    )

    #and boost includes as well
    find_package(Boost COMPONENTS)
    include_directories(${Boost_INCLUDE_DIRS})

    #build and install module to path
    add_library(${GRAS_TOOL_PROJECT} MODULE ${GRAS_TOOL_CPP_SOURCES})
    target_link_libraries(${GRAS_TOOL_PROJECT} ${PMC_LIBRARIES} ${GRAS_LIBRARIES})
    install(TARGETS ${GRAS_TOOL_PROJECT}
        LIBRARY DESTINATION ${GRAS_TOOL_MOD_DEST} COMPONENT ${GRAS_TOOL_COMPONENT} # .so file
        ARCHIVE DESTINATION ${GRAS_TOOL_MOD_DEST} COMPONENT ${GRAS_TOOL_COMPONENT} # .lib file
        RUNTIME DESTINATION ${GRAS_TOOL_MOD_DEST} COMPONENT ${GRAS_TOOL_COMPONENT} # .dll file
    )

    #TODO python module install

    #install GRC files
    install(
        FILES ${GRAS_TOOL_GRC_SOURCES}
        DESTINATION ${GRAS_TOOL_GRC_DEST}
        COMPONENT ${GRAS_TOOL_COMPONENT}
    )

endfunction(GRAS_TOOL)
