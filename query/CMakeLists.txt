########################################################################
# Install rules
########################################################################
include(GrPython)

GR_PYTHON_INSTALL(
    FILES
    __init__.py
    DESTINATION ${GR_PYTHON_DIR}/gras/query
    COMPONENT ${GRAS_COMP_PYTHON}
)

INSTALL(
    FILES
    main.html
    main.js
    utils.js
    chart_factory.js
    chart_overhead_compare.js
    chart_overall_throughput.js
    chart_handler_breakdown.js
    chart_port_counters.js
    chart_global_counters.js
    chart_port_downtime.js
    chart_topology_display.js
    main.css
    DESTINATION ${GR_PYTHON_DIR}/gras/query
    COMPONENT ${GRAS_COMP_PYTHON}
)
