########################################################################
# Install cmake modules to known location
########################################################################
include(GrPython)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/GRASTool.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/GRASTool.cmake
@ONLY)
list(APPEND GRAS_TOOL_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/GRASTool.cmake
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/GRASTool.cmake
    DESTINATION share/gras/cmake
    COMPONENT ${GRAS_COMP_DEVEL}
)
