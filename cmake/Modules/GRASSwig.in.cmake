if(DEFINED __INCLUDED_GRAS_SWIG_CMAKE)
    return()
endif()
set(__INCLUDED_GRAS_SWIG_CMAKE TRUE)

include(GRASTool)

########################################################################
## GRAS_SWIG_TOOL cmake function - export new types for the loader
##
## Options:
## SOURCE - the swig .i file
## TARGET - target name for the build products
## DIRECTORY - name of installation directory or ${TARGET}
## COMPONENT - name of installation component or ${TARGET}
##
## External vars:
## GRAS_SWIG_TOOL_INCLUDES - list of additional include directories
## GRAS_SWIG_TOOL_LIBRARIES - list of additional libraries to link to
########################################################################
function(GRAS_SWIG_TOOL)

    find_package(SWIG)
    if(NOT SWIG_FOUND)
        return()
    endif()

    find_package(PythonLibs)
    if(NOT PYTHONLIBS_FOUND)
        return()
    endif()

    include(CMakeParseArguments)
    CMAKE_PARSE_ARGUMENTS(GRAS_SWIG_TOOL "" "SOURCE;TARGET;DIRECTORY;COMPONENT" "" ${ARGN})

    #give an install directory if not specified
    if(NOT GRAS_SWIG_TOOL_DIRECTORY)
        set(GRAS_SWIG_TOOL_DIRECTORY ${GRAS_SWIG_TOOL_TARGET})
    endif()

    #give the target a component name if not specified
    if(NOT GRAS_SWIG_TOOL_COMPONENT)
        set(GRAS_SWIG_TOOL_COMPONENT ${GRAS_SWIG_TOOL_TARGET})
    endif()

    #development and framework directories
    list(APPEND GRAS_SWIG_TOOL_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR})
    list(APPEND GRAS_SWIG_TOOL_INCLUDES ${PMC_SWIG_INCLUDE_DIRS})
    list(APPEND GRAS_SWIG_TOOL_INCLUDES ${GRAS_SWIG_INCLUDE_DIRS})
    list(APPEND GRAS_SWIG_TOOL_LIBRARIES ${PMC_LIBRARIES})
    list(APPEND GRAS_SWIG_TOOL_LIBRARIES ${GRAS_LIBRARIES})

    #and boost includes as well
    include_directories(${Boost_INCLUDE_DIRS})

    #setup python includes/libs
    include_directories(${PYTHON_INCLUDE_PATH})
    include_directories(${PYTHON_INCLUDE_DIRS})
    list(APPEND GRAS_SWIG_TOOL_LIBRARIES ${PYTHON_LIBRARIES})

    #include swig generation deps for compiling
    include_directories(${GRAS_SWIG_TOOL_INCLUDES})

    #set the C++ property on the swig .i file so it builds
    set_source_files_properties(${GRAS_SWIG_TOOL_SOURCE} PROPERTIES CPLUSPLUS ON)

    #setup swig flags
    foreach(inc ${GRAS_SWIG_TOOL_INCLUDES})
        list(APPEND CMAKE_SWIG_FLAGS "-I${inc}")
    endforeach(inc)
    set(CMAKE_SWIG_FLAGS -fvirtual -module ${GRAS_SWIG_TOOL_TARGET} ${CMAKE_SWIG_FLAGS})

    #register swig build
    include(UseSWIG)
    SWIG_ADD_MODULE(${GRAS_SWIG_TOOL_TARGET} python ${GRAS_SWIG_TOOL_SOURCE})
    SWIG_LINK_LIBRARIES(${GRAS_SWIG_TOOL_TARGET} ${GRAS_SWIG_TOOL_LIBRARIES})

    #install rules for generated module and py file
    set(GRAS_TOOL_PYTHON_DIR lib@LIBSUFFIX@/gras/python/${GRAS_SWIG_TOOL_DIRECTORY})
    install(
        TARGETS ${SWIG_MODULE_${GRAS_SWIG_TOOL_TARGET}_REAL_NAME}
        DESTINATION ${GRAS_TOOL_PYTHON_DIR}
        COMPONENT ${GRAS_SWIG_TOOL_COMPONENT}
    )
    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${GRAS_SWIG_TOOL_TARGET}.py
        DESTINATION ${GRAS_TOOL_PYTHON_DIR}
        COMPONENT ${GRAS_SWIG_TOOL_COMPONENT}
    )

endfunction(GRAS_SWIG_TOOL)
