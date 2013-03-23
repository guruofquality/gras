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
                    gras_update_throughput_chart(registry);
                    gras_update_time_compare_chart(registry);
                    gras_update_per_block_charts(registry);
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
    var overall_config = $('#overall_config').get(0);

    gras_setup_overall_chart(registry);
    gras_setup_overall_chart_pie(registry);

    gras_chart_factory_init();

    //init overall config gui element for rate
    var overall_rate = $('input[name="rate"]', overall_config);
    overall_rate.val(registry.overall_rate);
    overall_rate.change(function()
    {
        registry.overall_rate = overall_rate.val();
    });

    //init overall config gui element for activity
    registry.overall_active = true;
    var overall_active = $('input[name="active"]', overall_config);
    overall_active.attr('checked', registry.overall_active);
    overall_active.change(function()
    {
        registry.overall_active = overall_active.is(':checked');
        if (registry.overall_active) gras_query_stats(registry);
        else window.clearInterval(registry.timeout_handle);
    });

    //init overall config gui element for showing
    var overall_show = $('input[name="show"]', overall_config);
    overall_show.attr('checked', false);
    overall_show.change(function()
    {
        var chart = $('#overall_chart');
        gras_animate_show_hide(chart, overall_show.is(':checked'));
    });
    overall_show.change();

    gras_query_stats(registry);
}
