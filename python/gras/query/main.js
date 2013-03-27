/***********************************************************************
 * Some constants
 **********************************************************************/
var GRAS_CHARTS_STD_WIDTH = 250;

/***********************************************************************
 * Stats registry data structure
 **********************************************************************/
var GrasStatsRegistry = function()
{
    this.overall_rate = 2.0;
    this.overall_active = true;
    this.block_ids = new Array();
    this.top_id = 'top';
    this.online = true;
    this.offline_count = 0;
}

/***********************************************************************
 * Query stats
 **********************************************************************/
var gras_query_stats = function(registry)
{
    $.ajax({
        type: "GET",
        async: true,
        url: "/stats.json",
        dataType: "json",
        success: function(response)
        {
            registry.online = true;
            gras_chart_factory_online(registry);
            if (registry.overall_active)
            {
                gras_chart_factory_update(registry, response);

                registry.timeout_handle = window.setTimeout(function()
                {
                    gras_query_stats(registry);
                }, Math.round(1000/registry.overall_rate));
            }
        },
        error: function()
        {
            registry.online = false;
            gras_chart_factory_online(registry);
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
    //create a new registry - storage for gui state
    var registry = new GrasStatsRegistry();

    //query various server args
    $.getJSON('/args.json', function(data)
    {
        registry.top_id = data.name;
        $('#top_name').append(' - ' + registry.top_id);
        document.title += ' - ' + registry.top_id;
    });

    //query the stats for initial setup
    $.getJSON('/stats.json', function(data)
    {
        gras_chart_factory_setup(registry, data);
    });

    //start the query loop in the background
    gras_query_stats(registry);
}
