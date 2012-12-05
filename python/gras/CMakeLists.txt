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
set(GR_SWIG_LIBRARIES gras)
file(GLOB GR_SWIG_SOURCE_DEPS "${GRAS_SOURCE_DIR}/include/gras/*.i")

GR_SWIG_MAKE(GRAS_Tags GRAS_Tags.i)
GR_SWIG_MAKE(GRAS_Block GRAS_Block.i)
GR_SWIG_MAKE(GRAS_HierBlock GRAS_HierBlock.i)
GR_SWIG_MAKE(GRAS_ThreadPool GRAS_ThreadPool.i)
GR_SWIG_MAKE(GRAS_SBuffer GRAS_SBuffer.i)
GR_SWIG_MAKE(GRAS_IOSignature GRAS_IOSignature.i)
GR_SWIG_INSTALL(
    TARGETS
    GRAS_Tags
    GRAS_Block
    GRAS_HierBlock
    GRAS_ThreadPool
    GRAS_SBuffer
    GRAS_IOSignature
    DESTINATION ${GR_PYTHON_DIR}/gras
    COMPONENT ${GRAS_COMP_PYTHON}
)

########################################################################
# Install rules
########################################################################
GR_PYTHON_INSTALL(
    FILES __init__.py GRAS_Utils.py
    DESTINATION ${GR_PYTHON_DIR}/gras
    COMPONENT ${GRAS_COMP_PYTHON}
)
