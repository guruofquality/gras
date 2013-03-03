/***********************************************************************
 * visuals
 **********************************************************************/

var gras_setup_individual_charts = function(point)
{
    var charts = $('#individual_charts').get(0);
    $('block', point).each(function()
    {
        $('<input>').attr({
            type: 'checkbox',
            name: $(this).attr('id'),
            value: '1',
            checked: '1',
            onclick: function()
            {
                
            }
        }).appendTo(charts);
        $(charts).append($(this).attr('id') + '&nbsp;');
    });
}


var gras_update_throughput_chart = function(data)
{
    if (data.history.length == 1) return gras_setup_individual_charts(data.history[0]);
    if (data.history.length < 2) return;

    var ids = data.getBlockIds();
    var data_set = [['Throughput'].concat(ids)];
    for (var i = Math.max(data.history.length-10, 1); i < data.history.length; i++)
    {
        var row = new Array();
        row.push(i.toString());
        for (var j = 0; j < ids.length; j++)
        {
            row.push(gras_extract_throughput_delta(data.history[i-1], data.history[i], ids[j])/1e6);
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
var gras_query_stats = function(data)
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
                data.appendPoint(xml);
                gras_update_throughput_chart(data);
            }
            var onceHandle = window.setTimeout(function() {
              gras_query_stats(data);
            }, 500);
        }
    });
}

/***********************************************************************
 * Init
 **********************************************************************/
var gras_stats_main = function()
{
    var data = new GrasStatsRegistry();
    gras_query_stats(data);
}
