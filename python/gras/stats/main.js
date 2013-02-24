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

var gras_extract_total_items = function(point, id)
{
    var block_data = $('block[id="' + id + '"]', point);
    var total_items = 0;
    $('items_consumed,items_produced', block_data).each(function()
    {
        total_items += parseInt($(this).text());
    });
    return total_items;
}

var gras_extract_throughput_delta = function(p0, p1, id)
{
    var d0 = $('block[id="' + id + '"]', p0);
    var d1 = $('block[id="' + id + '"]', p1);
    var t0 = parseInt($('stats_time', d0).text());
    var t1 = parseInt($('stats_time', d1).text());
    var tps = parseInt($('tps', d0).text());
    var items0 = gras_extract_total_items(p0, id);
    var items1 = gras_extract_total_items(p1, id);
    return ((items1-items0)*tps)/(t1-t0);
}

var gras_extract_throughput = function(point, id)
{
    var block_data = $('block[id="' + id + '"]', point);
    var start_time = parseInt($('start_time', block_data).text());
    var stats_time = parseInt($('stats_time', block_data).text());
    var tps = parseInt($('tps', block_data).text());
    var total_items = gras_extract_total_items(point, id);
    return (total_items*tps)/(stats_time-start_time);
}

var gras_update_throughput_chart = function(history)
{
    if (history.length < 2) return;

    var ids = gras_extract_block_ids(history[0]);
    var data_set = [['Throughput'].concat(ids)];
    for (var i = Math.max(history.length-10, 1); i < history.length; i++)
    {
        var row = new Array();
        row.push(i.toString());
        for (var j = 0; j < ids.length; j++)
        {
            row.push(gras_extract_throughput_delta(history[i-1], history[i], ids[j])/1e6);
        }
        data_set.push(row);
    }


    var data = google.visualization.arrayToDataTable(data_set);

    var options = {
        title: 'Throughput per block over time',
        vAxis: {title: "rate (MIps)"},
        hAxis: {title: "time (seconds)"}
    };

    var chart = new google.visualization.LineChart($('#overall_charts').get(0));
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
            }, 500);
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
