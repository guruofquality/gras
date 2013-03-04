/***********************************************************************
 * charts and visualization stuff
 **********************************************************************/

var gras_setup_overall_chart = function(registry)
{
    var chart = new google.visualization.LineChart($('#overall_charts').get(0));
    var options = {
        title: 'Throughput per block over time',
        vAxis: {title: "rate (MIps)"},
        hAxis: {title: "time (seconds)"}
    };
    registry.overall_chart = chart;
    registry.overall_chart_options = options;
}

var gras_setup_individual_charts = function(registry)
{
    var point = registry.history[0];
    var div = $('#block_enables:first');
    var config = $('td:last', div);
    var count = 0;
    $.each(registry.getBlockIds(), function(index, id)
    {
        $(config).append('<label>' + id + '</label>');
        $('<input>').attr({
            type: 'checkbox',
            name: id,
            value: '1',
            checked: '1',
            onclick: function()
            {
                
            }
        }).appendTo(config);
        $(config).append('&nbsp;');
        count++;
        if (count == Math.round(registry.getBlockIds().length/2))
        {
            var table = $('table:last', div);
            $(table).append('<tr><td></td></tr>');
            config = $('td:last', div);
        }
    });

}


var gras_update_throughput_chart = function(registry)
{
    if (registry.history.length == 1) return gras_setup_individual_charts(registry);
    if (registry.history.length < 2) return;

    var ids = registry.getBlockIds();
    var data_set = [['Throughput'].concat(ids)];
    for (var i = Math.max(registry.history.length-10, 1); i < registry.history.length; i++)
    {
        var row = new Array();
        row.push(i.toString());
        for (var j = 0; j < ids.length; j++)
        {
            row.push(gras_extract_throughput_delta(registry.history[i-1], registry.history[i], ids[j])/1e6);
        }
        data_set.push(row);
    }

    var chart_data = google.visualization.arrayToDataTable(data_set);
    registry.overall_chart.draw(chart_data, registry.overall_chart_options);

}
