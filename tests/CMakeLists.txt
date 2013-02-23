########################################################################
# Unit tests!
########################################################################

include(GrTest)

include(GrPython)
set(PYTHON_EXECUTABLE ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B})

GR_ADD_TEST(block_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/block_test.py)
GR_ADD_TEST(hier_block_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/hier_block_test.py)
GR_ADD_TEST(thread_pool_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/thread_pool_test.py)
GR_ADD_TEST(sbuffer_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/sbuffer_test.py)
