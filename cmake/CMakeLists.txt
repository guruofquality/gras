########################################################################
# Install cmake modules to known location
########################################################################
install(
    FILES GRASTool.cmake
    DESTINATION share/gras/cmake
    COMPONENT ${GRAS_COMP_DEVEL}
)
