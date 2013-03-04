/***********************************************************************
 * charts and visualization stuff
 **********************************************************************/

var gras_setup_overall_chart = function(registry)
{
    var div = $('#overall_charts:first');
    var td = $('td:last', div);
    var chart = new google.visualization.LineChart(td.get(0));
    registry.overall_chart = chart;
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
        var input = $('<input />').attr({
            type: 'checkbox',
            name: id
        });
        registry.block_enables[id] = true;
        input.attr('checked', registry.block_enables[id]);
        input.change(function()
        {
            registry.block_enables[id] = input.is(':checked');
        });
        $(config).append(input);
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

    var ids = new Array();
    $.each(registry.getBlockIds(), function(index, id)
    {
        if (registry.block_enables[id]) ids.push(id);
    });

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
    var options = {
        //title: 'Throughput per block over time',
        vAxis: {title: "rate (MIps)"},
        hAxis: {title: "time (seconds)"},
        width:$('#page').width()*0.9,
        height:'300'
    };
    registry.overall_chart.draw(chart_data, options);

}
