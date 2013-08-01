########################################################################
# Install cmake modules to known location
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/GRASTool.in.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/GRASTool.cmake
@ONLY)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/GRASSwig.in.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/GRASSwig.cmake
@ONLY)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/GRASTool.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/GRASSwig.cmake
        GRASCommon.cmake
        GRASTest.cmake
        GRASPython.cmake
    DESTINATION share/gras/cmake/Modules
    COMPONENT ${GRAS_COMP_DEVEL}
)
