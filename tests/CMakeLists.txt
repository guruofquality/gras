########################################################################
# Unit tests!
########################################################################

include(GrTest)

########################################################################
# unit test suite
########################################################################
find_package(Boost COMPONENTS unit_test_framework serialization)

if (NOT Boost_FOUND)
    return()
endif()

set(test_sources
    callable_test.cpp
    chrono_time_test.cpp
    block_props_test.cpp
    factory_test.cpp
    serialize_tags_test.cpp
)

include_directories(${GRAS_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND GR_TEST_LIBRARY_DIRS ${Boost_LIBRARY_DIRS})

#turn each test cpp file into an executable with an int main() function
add_definitions(-DBOOST_TEST_DYN_LINK -DBOOST_TEST_MAIN)

#for each source: build an executable, register it as a test
foreach(test_source ${test_sources})
    get_filename_component(test_name ${test_source} NAME_WE)
    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name} ${Boost_LIBRARIES} ${GRAS_LIBRARIES})
    GR_ADD_TEST(${test_name}_cpp ${test_name})
endforeach(test_source)

########################################################################
# Python unit tests
########################################################################
include(GrPython)
set(PYTHON_EXECUTABLE ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B})

GR_ADD_TEST(block_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/block_test.py)
GR_ADD_TEST(hier_block_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/hier_block_test.py)
GR_ADD_TEST(thread_pool_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/thread_pool_test.py)
GR_ADD_TEST(sbuffer_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/sbuffer_test.py)
GR_ADD_TEST(query_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/query_test.py)
GR_ADD_TEST(block_props_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/block_props_test.py)
GR_ADD_TEST(time_tags_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/time_tags_test.py)
