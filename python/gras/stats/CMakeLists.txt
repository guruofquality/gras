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
    chart_factory.js
    chart_overhead_compare.js
    chart_overall_throughput.js
    chart_handler_breakdown.js
    main.css
    DESTINATION ${GR_PYTHON_DIR}/gras/stats
    COMPONENT ${GRAS_COMP_PYTHON}
)
