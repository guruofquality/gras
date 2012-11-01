########################################################################
# Unit tests!
########################################################################

include(GrTest)

include(GrPython)
set(PYTHON_EXECUTABLE ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B})

list(APPEND GR_TEST_PYTHON_DIRS ${GRAS_SOURCE_DIR}/python)
list(APPEND GR_TEST_PYTHON_DIRS ${GRAS_BINARY_DIR}/python)
list(APPEND GR_TEST_PYTHON_DIRS ${GRAS_BINARY_DIR}/python/gras)
list(APPEND GR_TEST_PYTHON_DIRS ${GRAS_BINARY_DIR}/python/gras/${CMAKE_BUILD_TYPE})
GR_ADD_TEST(hier_block ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/hier_block_test.py)
GR_ADD_TEST(thread_pool ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/thread_pool_test.py)
