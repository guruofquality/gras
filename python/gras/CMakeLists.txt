########################################################################
# Include swig generation macros
########################################################################
find_package(SWIG)
if(NOT SWIG_FOUND)
    return()
endif()

find_package(PythonLibs)
if(NOT PYTHONLIBS_FOUND)
    return()
endif()

include(UseSWIG)
include(GrPython)

########################################################################
# Local includes
########################################################################
list(APPEND CMAKE_SWIG_INC_FLAGS "-I${PMC_SOURCE_DIR}/include")
list(APPEND CMAKE_SWIG_INC_FLAGS "-I${GRAS_SOURCE_DIR}/include")
include_directories(${GRAS_INCLUDE_DIRS})

########################################################################
# Python library setup
########################################################################
include_directories(${PYTHON_INCLUDE_PATH})
include_directories(${PYTHON_INCLUDE_DIRS})

########################################################################
# Finding Boost headers
########################################################################
find_package(Boost)
include_directories(${Boost_INCLUDE_DIRS})

########################################################################
# swig build modules
########################################################################
set(gras_swig_modules
    GRAS_Tags
    GRAS_TimeTag
    GRAS_Element
    GRAS_Factory
    GRAS_Block
    GRAS_HierBlock
    GRAS_PyBlock
    GRAS_PyHierBlocks
    GRAS_TopBlock
    GRAS_ThreadPool
    GRAS_SBuffer
)

file(GLOB pmc_i_files "${PMC_SOURCE_DIR}/include/PMC/*.i")
file(GLOB gras_i_files "${GRAS_SOURCE_DIR}/include/gras/*.i")

foreach(gras_swig_module ${gras_swig_modules})
    message(STATUS "Configuring swig python module ${gras_swig_module}...")

    #set the C++ property on the swig .i file so it builds
    set_source_files_properties(${gras_swig_module}.i PROPERTIES CPLUSPLUS ON)

    set(CMAKE_SWIG_FLAGS -fvirtual -module ${gras_swig_module} ${CMAKE_SWIG_INC_FLAGS})
    set(SWIG_MODULE_${gras_swig_module}_EXTRA_DEPS ${pmc_i_files} ${gras_i_files})
    SWIG_ADD_MODULE(${gras_swig_module} python ${gras_swig_module}.i)
    SWIG_LINK_LIBRARIES(${gras_swig_module} ${GRAS_LIBRARIES} ${PYTHON_LIBRARIES})

    install(
        TARGETS ${SWIG_MODULE_${gras_swig_module}_REAL_NAME}
        DESTINATION ${GR_PYTHON_DIR}/gras
        COMPONENT ${GRAS_COMP_PYTHON}
    )
    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${gras_swig_module}.py
        DESTINATION ${GR_PYTHON_DIR}/gras
        COMPONENT ${GRAS_COMP_PYTHON}
    )
endforeach(gras_swig_module)

########################################################################
# install other python files
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/GRAS_Loader.py
    ${CMAKE_CURRENT_BINARY_DIR}/GRAS_Loader.py
@ONLY)

GR_PYTHON_INSTALL(
    FILES
    __init__.py
    GRAS_Utils.py
    GRAS_TestUtils.py
    ${CMAKE_CURRENT_BINARY_DIR}/GRAS_Loader.py
    DESTINATION ${GR_PYTHON_DIR}/gras
    COMPONENT ${GRAS_COMP_PYTHON}
)
