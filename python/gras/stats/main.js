/***********************************************************************
 * Utility functions for stats
 **********************************************************************/
var gras_extract_block_ids = function(point)
{
    var ids = new Array();
    $('block', point).each(function()
    {
        ids.push($(this).attr('id'));
    });
    return ids;
}

var gras_extract_throughput = function(point, id)
{
    var block_data = $('block[id="' + id + '"]', point);
    var start_time = parseInt($('start_time', block_data).text());
    var stats_time = parseInt($('stats_time', block_data).text());
    var tps = parseInt($('tps', block_data).text());
    var total_items = 0;
    $('items_consumed,items_produced', block_data).each(function()
    {
        total_items += parseInt($(this).text());
    });
    return (total_items*tps)/(stats_time-start_time);
}

var gras_update_throughput_chart = function(history)
{
    var ids = gras_extract_block_ids(history[0]);
    var data_set = [['Throughput'].concat(ids)];
    for (var i = Math.max(history.length-10, 0); i < history.length; i++)
    {
        var row = new Array();
        row.push(i.toString());
        for (var j = 0; j < ids.length; j++)
        {
            row.push(gras_extract_throughput(history[i], ids[j])/1e6);
        }
        data_set.push(row);
    }


    var data = google.visualization.arrayToDataTable(data_set);

    var options = {
        title: 'Throughput per block over time',
        vAxis: {title: "rate (MIps)"},
        hAxis: {title: "time (seconds)"}
    };

    var chart = new google.visualization.LineChart(document.getElementById('chart_div'));
    chart.draw(data, options);

}

/***********************************************************************
 * Query stats
 **********************************************************************/
var gras_query_stats = function(history)
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
                history.push(xml);
                gras_update_throughput_chart(history);
            }
            var onceHandle = window.setTimeout(function() {
              gras_query_stats(history);
            }, 1000);
        }
    });
}

/***********************************************************************
 * Init
 **********************************************************************/
var gras_stats_main = function()
{
    var history = new Array();
    gras_query_stats(history);
}
