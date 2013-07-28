########################################################################
# Install cmake modules to known location
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/GRASTool.in.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/GRASTool.cmake
@ONLY)
list(APPEND GRAS_TOOL_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/GRASTool.cmake
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/GRASTool.cmake
        ${CMAKE_CURRENT_SOURCE_DIR}/GrTest.cmake
    DESTINATION share/gras/cmake/Modules
    COMPONENT ${GRAS_COMP_DEVEL}
)
