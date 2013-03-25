/***********************************************************************
 * Some constants
 **********************************************************************/
var GRAS_CHARTS_STD_WIDTH = 250;

/***********************************************************************
 * Stats registry data structure
 **********************************************************************/
var GrasStatsRegistry = function()
{
    this.init = false;
    this.overall_rate = 2.0;
    this.overall_active = true;
    this.block_ids = new Array();
    this.top_id = 'top';
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
                        gras_chart_factory_setup(registry, xml);
                        try{gras_chart_load(registry);}catch(e){}
                        registry.init = true;
                    }
                    $.each(registry.active_charts, function(index, chart_info)
                    {
                        chart_info.chart.update(xml);
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
