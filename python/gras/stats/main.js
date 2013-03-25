/***********************************************************************
 * Stats registry data structure
 **********************************************************************/
var GrasStatsRegistry = function()
{
    this.init = false;
    this.ids = new Array();
    this.enables = new Array();
    this.block_enables = new Array();
    this.overall_rate = 2.0;
    this.overall_active = true;
    this.active_charts = new Array();
}

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
                    if (!registry.init)
                    {
                        gras_chart_factory_setup(xml);
                        registry.init = true;
                    }
                    $.each(registry.active_charts, function(index, chart)
                    {
                        chart.update(xml);
                    });
                }

                registry.timeout_handle = window.setTimeout(function()
                {
                    gras_query_stats(registry);
                }, Math.round(1000/registry.overall_rate));
            }
        },
        error: function()
        {
            registry.timeout_handle = window.setTimeout(function()
            {
                gras_query_stats(registry);
            }, 1000);
        },
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
