/***********************************************************************
 * Utility functions for stats
 **********************************************************************/
var gras_extract_total_items = function(point, id)
{
    var block_data = point.blocks[id];
    var total_items = 0;
    $.each(block_data.items_produced, function(index, value)
    {
        total_items += value;
    });
    $.each(block_data.items_consumed, function(index, value)
    {
        total_items += value;
    });
    return total_items;
}

var gras_extract_throughput_delta = function(p0, p1, id)
{
    var d0 = p0.blocks[id];
    var d1 = p1.blocks[id];
    var t0 = d0.stats_time;
    var t1 = d1.stats_time;
    var tps = d0.tps;
    var items0 = gras_extract_total_items(p0, id);
    var items1 = gras_extract_total_items(p1, id);
    return ((items1-items0)*tps)/(t1-t0);
}

var gras_extract_throughput = function(point, id)
{
    var block_data = point.blocks[id];
    var start_time = block_data.start_time;
    var stats_time = block_data.stats_time;
    var tps = block_data.tps;
    var total_items = gras_extract_total_items(point, id);
    return (total_items*tps)/(stats_time-start_time);
}

var gras_extract_stat_time_delta = function(p0, p1)
{
    var t0 = p0.now;
    var t1 = p1.now;
    var tps = p0.tps;
    return (t1-t0)/(tps);
}

var gras_extract_percent_times = function(point, id)
{
    var block_data = point.blocks[id];
    var data = {
        prep: block_data.total_time_prep,
        work: block_data.total_time_work,
        post: block_data.total_time_post,
        input: block_data.total_time_input,
        output: block_data.total_time_output,
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

var gras_error_dialog = function(error_title, error_text)
{
    $("#div-dialog-warning").text(error_text);
    $("#div-dialog-warning").dialog({
        title: error_title,
        resizable: false,
        height: 160,
        modal: true,
        buttons: {
            "Ok" : function () {
                $(this).dialog("close");
            }
        }
    }).parent().addClass("ui-state-error");
}
