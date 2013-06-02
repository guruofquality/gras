/***********************************************************************
 * Stats registry data structure
 **********************************************************************/
var GrasStatsRegistry = function()
{
    this.overall_rate = 3.0;
    this.overall_active = true;
    this.block_ids = new Array();
    this.top_id = 'top';
    this.online = true;
    this.offline_count = 0;
}

/***********************************************************************
 * Server offline animation
 **********************************************************************/
function gras_handle_offline(registry)
{
    if (!registry.online) registry.offline_count++;
    if (registry.online) $('#page').css('background-color', '#EEEEFF');
    else if (registry.offline_count%2 == 0) $('#page').css('background-color', '#FF4848');
    else if (registry.offline_count%2 == 1) $('#page').css('background-color', '#EEEEFF');
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
        traditional: true, //needed to parse data
        data: {blocks:gras_chart_factory_active_blocks(registry)},
        success: function(response)
        {
            registry.online = true;
            gras_handle_offline(registry);
            if (registry.overall_active) gras_chart_factory_update(registry, response);

            var timeout = registry.overall_active? Math.round(1000/registry.overall_rate) : 1000;
            window.setTimeout(function()
            {
                gras_query_stats(registry);
            }, timeout);
        },
        error: function()
        {
            registry.online = false;
            gras_handle_offline(registry);
            window.setTimeout(function()
            {
                gras_query_stats(registry);
            }, 1000);
        },
    });
}

/***********************************************************************
 * Init
 **********************************************************************/
var gras_query_main = function()
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

    //initialize the charts factory
    gras_chart_factory_init(registry);
}
