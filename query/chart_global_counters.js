function GrasChartGlobalCounts(args, panel)
{
    //input checking
    if (args.block_ids.length != 0) throw gras_error_dialog(
        "GrasChartGlobalCounts",
        "Error making global counts chart.\n"+
        "Do not specify any blocks for this chart."
    );

    //settings
    this.div = $('<div />').attr({class:'chart_total_counts'});
    $(panel).append(this.div);
    this.title = "Global Counters"
}

GrasChartGlobalCounts.prototype.update = function(point)
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
        ['Allocated', 'bytes', 'default_allocator_bytes_allocated'],
        ['Peak size', 'bytes', 'default_allocator_peak_bytes_allocated'],
        ['Num mallocs', '', 'default_allocator_allocation_count'],

        ['Total msgs', '', 'framework_counter_messages_processed'],
        ['Thread yields', '', 'framework_counter_yields'],
        ['Local pushes', '', 'framework_counter_local_pushes'],
        ['Shared pushes', '', 'framework_counter_shared_pushes'],
        ['Msg queue max', '', 'framework_counter_mailbox_queue_max'],
        ['Max latency', 'ms', 'framework_counter_queue_latency_max'],
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
