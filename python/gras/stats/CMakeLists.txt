########################################################################
# Install rules
########################################################################
include(GrPython)

GR_PYTHON_INSTALL(
    FILES
    __init__.py
    DESTINATION ${GR_PYTHON_DIR}/gras/stats
    COMPONENT ${GRAS_COMP_PYTHON}
)

INSTALL(
    FILES
    main.html
    main.js
    registry.js
    utils.js
    charts.js
    main.css
    DESTINATION ${GR_PYTHON_DIR}/gras/stats
    COMPONENT ${GRAS_COMP_PYTHON}
)
