/***********************************************************************
 * charts and visualization stuff
 **********************************************************************/

var gras_animate_show_hide = function(elem, show)
{
    if (show) elem.slideDown("fast");
    else elem.slideUp("fast");
}

var gras_setup_overall_chart = function(registry)
{
    var div = $('#overall_chart:first');
    var td = $('td:last', div);
    var chart = new google.visualization.LineChart(td.get(0));
    registry.overall_chart = chart;
}

var gras_setup_per_block_enable_checkbox = function(elem, id, registry)
{
    $(elem).append('<label>' + id + '</label>');
    var input = $('<input />').attr({
        type: 'checkbox',
        name: id
    });
    registry.block_enables[id] = false;
    input.attr('checked', registry.block_enables[id]);
    input.change(function()
    {
        registry.block_enables[id] = input.is(':checked');
        //gras_update_throughput_chart(registry);
        var div = $('#per_block_charts');
        gras_animate_show_hide($('table[name="' + id + '"]', div), registry.block_enables[id]);
    });
    $(elem).append(input);
    $(elem).append('&nbsp;');
    input.change();
}

var gras_setup_per_block_charts = function(id, registry)
{
    var div = $('#per_block_charts');
    var table = $('<table />');
    table.attr('name', id);
    div.append(table);
    table.append('<tr><th></th></tr>');
    $('th:last', table).text(id);
    table.append('<tr><td></td></tr>');
    var td = $('td:last', table);
    var chart = new google.visualization.PieChart(td.get(0));
    registry.block_charts[id] = chart;
}

var gras_setup_individual_charts = function(registry)
{
    var point = registry.history[0];
    var div = $('#block_enables:first');
    var config = $('td:last', div);
    var count = 0;
    $.each(registry.getBlockIds(), function(index, id)
    {
        gras_setup_per_block_charts(id, registry);
        gras_setup_per_block_enable_checkbox(config, id, registry);
        count++;
        if (count == Math.round(registry.getBlockIds().length/2))
        {
            var table = $('table:last', div);
            $(table).append('<tr><td></td></tr>');
            config = $('td:last', div);
        }
    });

}

var gras_update_per_block_chart = function(id, registry)
{
    var point = registry.history[registry.history.length-1];
    var percents = gras_extract_percent_times(point, id);
    var data = google.visualization.arrayToDataTable([
          ['Task', 'Percent'],
          ['Work prep', percents['prep']],
          ['Work task', percents['work']],
          ['Work post', percents['post']],
          ['Input tasks', percents['input']],
          ['Output tasks', percents['output']],
    ]);

    var options = {
        width:$('#page').width()/5,
        chartArea:{left:5,top:0,right:5,bottom:0,width:"100%",height:"100%"},
    };

    var chart = registry.block_charts[id];
    chart.draw(data, options);
}

var gras_update_per_block_charts = function(registry)
{
    if (registry.history.length == 0) return;
    $.each(registry.getBlockIds(), function(index, id)
    {
        gras_update_per_block_chart(id, registry);
    });
}

var gras_update_throughput_chart = function(registry)
{
    if (registry.history.length == 1)
    {
        var id = $('gras_stats:first', registry.history[0]).attr('id');
        $('h1:first').append(' - ' + id);
        gras_setup_individual_charts(registry);
        return;
    }
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
        width:$('#page').width()*0.9,
        height:'250',
        chartArea:{left:0,top:0,right:0,bottom:0,width:"100%",height:"85%"},
        legend: {'position': 'bottom'},
    };
    registry.overall_chart.draw(chart_data, options);

}
