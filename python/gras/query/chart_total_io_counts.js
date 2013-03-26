function GrasChartTotalIoCounts(args)
{
    //input checking
    if (args.block_ids.length != 1) throw gras_error_dialog(
        "GrasChartTotalIoCounts",
        "Error making total IO counts chart.\n"+
        "Specify only one block for this chart."
    );

    //settings
    this.block_id = args.block_ids[0];
    this.div = $('<div />').attr({class:'chart_total_io_counts'});
    $(args.panel).append(this.div);
    this.title = "I/O Totals - " + this.block_id;
}

GrasChartTotalIoCounts.prototype.update = function(point)
{
    var block_data = point.blocks[this.block_id];
    var ul = $('<ul />');
    $('ul', this.div).remove(); //clear old lists
    this.div.append(ul);

    {
        var init_time = parseInt(block_data.init_time);
        var stats_time = parseInt(block_data.stats_time);
        var tps = parseInt(block_data.tps);
        var duration = (stats_time - init_time)/tps;
        var li = $('<li />');
        var strong = $('<strong />').text('Elapsed' + ': ');
        var span = $('<span />').text(duration.toFixed(2).toString() + ' secs');
        li.append(strong);
        li.append(span);
        ul.append(li);
    }

    var stuff = [
        ['Input', 'items', 'items_consumed'],
        ['Input', 'tags', 'tags_consumed'],
        ['Input', 'msgs', 'msgs_consumed'],
        ['Output', 'items', 'items_produced'],
        ['Output', 'tags', 'tags_produced'],
        ['Output', 'msgs', 'msgs_produced'],
    ];

    $.each(stuff, function(contents_i, contents)
    {
        var dir = contents[0];
        var units = contents[1];
        var key = contents[2];
        $.each(block_data[key], function(index, count)
        {
            var li = $('<li />');
            var strong = $('<strong />').text(dir + index.toString() + ': ');
            var span = $('<span />').text(count.toString() + ' ' + units);
            li.append(strong);
            li.append(span);
            if (count > 0) ul.append(li);
        });
    });
}
