function GrasChartPortCounts(args, panel)
{
    //input checking
    if (args.block_ids.length != 1) throw gras_error_dialog(
        "GrasChartPortCounts",
        "Error making total port counts chart.\n"+
        "Specify only one block for this chart."
    );

    //settings
    this.block_id = args.block_ids[0];
    this.div = $('<div />').attr({class:'chart_total_counts'});
    $(panel).append(this.div);
    this.title = "Port Counters - " + this.block_id;
}

GrasChartPortCounts.prototype.update = function(point)
{
    var block_data = point.blocks[this.block_id];
    if (!block_data) return;
    var ul = $('<ul />');
    $('ul', this.div).remove(); //clear old lists
    this.div.append(ul);

    function make_entry(strong, span)
    {
        var li = $('<li />');
        var strong = $('<strong />').text(strong + ": ");
        var span = $('<span />').text(span);
        li.append(strong);
        li.append(span);
        ul.append(li);
    }

    //create total time elapsed entry
    {
        var init_time = block_data.init_time;
        var stats_time = block_data.stats_time;
        var tps = block_data.tps;
        var duration = (stats_time - init_time)/tps;
        make_entry('Elapsed', duration.toFixed(2).toString() + ' secs');
    }

    var stuff = [
        ['Enque', 'items', 'items_enqueued'],
        ['Enque', 'tags', 'tags_enqueued'],
        ['Enque', 'msgs', 'msgs_enqueued'],
        ['Input', 'items', 'items_consumed'],
        ['Input', 'tags', 'tags_consumed'],
        ['Input', 'msgs', 'msgs_consumed'],
        ['Output', 'items', 'items_produced'],
        ['Output', 'tags', 'tags_produced'],
        ['Output', 'msgs', 'msgs_produced'],
        ['Copied', 'bytes', 'bytes_copied'],
    ];

    $.each(stuff, function(contents_i, contents)
    {
        var dir = contents[0];
        var units = contents[1];
        var key = contents[2];
        $.each(block_data[key], function(index, count)
        {
            if (count > 0)
            {
                make_entry(dir + index.toString(), count.toString() + ' ' + units);
            }
        });
    });

    var actor_depth = block_data.actor_queue_depth;
    if (actor_depth > 10) //only show if its large
    {
        make_entry('Actor depth', actor_depth.toString() + ' msgs');
    }
}
