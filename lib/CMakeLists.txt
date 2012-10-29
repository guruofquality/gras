########################################################################
# Setup the GRAS library build
########################################################################
include_directories(${GRAS_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

########################################################################
# Setup Boost
########################################################################
if(UNIX AND EXISTS "/usr/lib64")
    list(APPEND BOOST_LIBRARYDIR "/usr/lib64") #fedora 64-bit fix
endif(UNIX AND EXISTS "/usr/lib64")

set(Boost_ADDITIONAL_VERSIONS
    "1.35.0" "1.35" "1.36.0" "1.36" "1.37.0" "1.37" "1.38.0" "1.38" "1.39.0" "1.39"
    "1.40.0" "1.40" "1.41.0" "1.41" "1.42.0" "1.42" "1.43.0" "1.43" "1.44.0" "1.44"
    "1.45.0" "1.45" "1.46.0" "1.46" "1.47.0" "1.47" "1.48.0" "1.48" "1.49.0" "1.49"
    "1.50.0" "1.50" "1.51.0" "1.51" "1.52.0" "1.52" "1.53.0" "1.53" "1.54.0" "1.54"
    "1.55.0" "1.55" "1.56.0" "1.56" "1.57.0" "1.57" "1.58.0" "1.58" "1.59.0" "1.59"
    "1.60.0" "1.60" "1.61.0" "1.61" "1.62.0" "1.62" "1.63.0" "1.63" "1.64.0" "1.64"
    "1.65.0" "1.65" "1.66.0" "1.66" "1.67.0" "1.67" "1.68.0" "1.68" "1.69.0" "1.69"
)
find_package(Boost COMPONENTS)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND GRAS_LIBRARIES ${Boost_LIBRARIES})

########################################################################
# Setup Theron Deps
########################################################################
set(THERON_SOURCE_DIR ${GRAS_SOURCE_DIR}/Theron)
include(${THERON_SOURCE_DIR}/TheronSetup.cmake)
include_directories(${THERON_INCLUDE_DIRS})
link_directories(${THERON_LIBRARY_DIRS})
add_definitions(${THERON_DEFINES})

list(APPEND GRAS_LIBRARIES ${THERON_LIBRARIES})
list(APPEND GRAS_SOURCES ${THERON_SOURCES})

########################################################################
# Setup Apology Deps
########################################################################
include_directories(${GRAS_SOURCE_DIR}/Apology/include)
include_directories(${GRAS_SOURCE_DIR}/Apology/lib)

file(GLOB apology_sources "${GRAS_SOURCE_DIR}/Apology/lib/*.cpp")
list(APPEND GRAS_SOURCES ${apology_sources})

########################################################################
# Append gras-core library sources
########################################################################
list(APPEND GRAS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/debug.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/element.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sbuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tags.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block_actor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block_task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/theron_allocator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block_allocator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/block_handlers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/topology_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/input_handlers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/output_handlers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hier_block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/top_block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/register_messages.cpp
)

########################################################################
# Build library
########################################################################
include(GrMiscUtils)

add_library(gras SHARED ${GRAS_SOURCES})
target_link_libraries(gras ${GRAS_LIBRARIES})
set(GR_LIBRARY_DIR lib)
GR_LIBRARY_FOO(gras RUNTIME_COMPONENT ${GRAS_COMP_RUNTIME} DEVEL_COMPONENT ${GRAS_COMP_DEVEL})
