########################################################################
# Include swig generation macros
########################################################################
find_package(SWIG)
find_package(PythonLibs)
if(NOT SWIG_FOUND OR NOT PYTHONLIBS_FOUND)
    return()
endif()
include(GrSwig)
include(GrPython)
find_package(Boost) #for headers

########################################################################
# setup SWIG build
########################################################################
list(APPEND GR_SWIG_INCLUDE_DIRS ${GRAS_INCLUDE_DIRS})
list(APPEND GR_SWIG_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})

GR_SWIG_MAKE(GRASElements GRASElements.i)
GR_SWIG_INSTALL(
    TARGETS GRASElements
    DESTINATION ${GR_PYTHON_DIR}/gras
    COMPONENT ${GRAS_COMP_PYTHON}
)

########################################################################
# Install rules
########################################################################
GR_PYTHON_INSTALL(
    FILES __init__.py
    DESTINATION ${GR_PYTHON_DIR}/gras
    COMPONENT ${GRAS_COMP_PYTHON}
)
