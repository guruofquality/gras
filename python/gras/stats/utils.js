/***********************************************************************
 * Utility functions for stats
 **********************************************************************/
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

var gras_extract_percent_times = function(point, id)
{
    var block_data = $('block[id="' + id + '"]', point);
    var data = {
        prep: parseInt($('total_time_prep', block_data).text()),
        work: parseInt($('total_time_work', block_data).text()),
        post: parseInt($('total_time_post', block_data).text()),
        input: parseInt($('total_time_input', block_data).text()),
        output: parseInt($('total_time_output', block_data).text()),
    };
    var total = 0;
    $.each(data, function(key, val)
    {
        total += val;
    });
    data['total'] = total;
    return data;
}

var gras_animate_show_hide = function(elem, show)
{
    if (show) elem.slideDown("fast");
    else elem.slideUp("fast");
}
