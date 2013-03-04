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
            if ($(xml, "gras_stats") !== undefined)
            {
                registry.appendPoint(xml);
                gras_update_throughput_chart(registry);
            }
            var onceHandle = window.setTimeout(function()
            {
              gras_query_stats(registry);
            }, (1.0/registry.overall_rate));
        }
    });
}

/***********************************************************************
 * Init
 **********************************************************************/
var gras_stats_main = function()
{
    var registry = new GrasStatsRegistry();
    var overall_config = $('#overall_config').get(0);
    var overall_rate = $('input[name="update_rate"]', overall_config);
    overall_rate.val(registry.overall_rate);
    overall_rate.change(function()
    {
        registry.overall_rate = overall_rate.val();
    });
    gras_setup_overall_chart(registry);
    gras_query_stats(registry);
}
