/***********************************************************************
 * Query stats
 **********************************************************************/
var gras_query_stats = function(registry)
{
    $.ajax({
        type: "GET",
        async: true,
        url: "/stats.xml",
        dataType: "xml",
        success: function(xml)
        {
            if (registry.overall_active)
            {
                if ($(xml, "gras_stats") !== undefined)
                {
                    registry.appendPoint(xml);
                    gras_chart_factory_setup(registry);
                    $.each(gras_chart_active_registry, function(index, chart)
                    {
                        chart.update(registry.history);
                    });
                }

                registry.timeout_handle = window.setTimeout(function()
                {
                    gras_query_stats(registry);
                }, Math.round(1000/registry.overall_rate));
            }
        }
    });
}

/***********************************************************************
 * Init
 **********************************************************************/
var gras_stats_main = function()
{
    var registry = new GrasStatsRegistry();
    gras_chart_factory_init(registry);
    gras_query_stats(registry);
}
