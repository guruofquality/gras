########################################################################
# Setup the GRAS Tool executable
########################################################################
list(APPEND GRAS_TOOL_SOURCES
    gras_tool.cpp
    tool_process.cpp
)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

########################################################################
# Generate source with build dirs
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/tool_dirs.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/tool_dirs.cpp
@ONLY)
list(APPEND GRAS_TOOL_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/tool_dirs.cpp
)

########################################################################
# Setup Boost
########################################################################
find_package(Boost COMPONENTS program_options system filesystem)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND GRAS_TOOL_LIBRARIES ${Boost_LIBRARIES})

########################################################################
# Build executable
########################################################################
add_executable(gras_tool ${GRAS_TOOL_SOURCES})
target_link_libraries(gras_tool ${GRAS_TOOL_LIBRARIES})

install(TARGETS gras_tool
    DESTINATION bin
    COMPONENT ${GRAS_COMP_RUNTIME}
)
