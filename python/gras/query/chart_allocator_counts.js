function GrasChartAllocatorCounts(args, panel)
{
    //input checking
    if (args.block_ids.length != 0) throw gras_error_dialog(
        "GrasChartAllocatorCounts",
        "Error making allocator counts chart.\n"+
        "Do not specify any blocks for this chart."
    );

    //settings
    this.div = $('<div />').attr({class:'chart_total_io_counts'});
    $(panel).append(this.div);
    this.title = "Theron allocator counts"
}

GrasChartAllocatorCounts.prototype.update = function(point)
{
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

    var stuff = [
        ['Allocated', 'bytes', 'bytes_allocated'],
        ['Peak size', 'bytes', 'peak_bytes_allocated'],
        ['Malloc\'d', 'times', 'allocation_count'],
    ];

    var entries = 0;
    $.each(stuff, function(contents_i, contents)
    {
        var dir = contents[0];
        var units = contents[1];
        var key = contents[2];
        var count = (key in point)? point[key] : 0;
        if (count > 0)
        {
            make_entry(dir, count.toString() + ' ' + units);
            entries++;
        }
    });
    if (entries == 0) make_entry("Counts", "none");
}
