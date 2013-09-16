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
    this.title = "Global Counters";

    this.counts_ul = $('<ul />');
    this.div.append(this.counts_ul);
    this.tabs_top = $('<div />');
    this.div.append(this.tabs_top);
    this.make_tabbed_thread_pool_counts(0);
}

GrasChartGlobalCounts.prototype.make_tabbed_thread_pool_counts = function(num)
{
    var key = 'chart_global_counter_tabs' + Math.random().toString(36).substring(7);
    var tabs = $('<div />').attr('id', key);

    //insert the tabs into the upper div
    this.tabs_top.empty().append(tabs);

    var tabs_hdr = $('<ul />');
    tabs.append(tabs_hdr);

    //create headers and body
    this.tp_infos = new Array();
    for (var i = 0; i < num; i++)
    {
        var href = key + '-' + i.toString();
        var tab_a = $('<a />').attr('href', '#' + href).text(i.toString());
        var tab_li = $('<li />');
        tab_li.append(tab_a);
        tabs_hdr.append(tab_li);
        var tab_div = $('<div />').attr('id', href);
        tabs.append(tab_div);
        this.tp_infos.push(tab_div);
    }

    tabs.tabs(); //generates jquery ui tabs
}

GrasChartGlobalCounts.prototype.update = function(point)
{
    var self = this;

    function make_entry(strong, span)
    {
        var li = $('<li />');
        var strong = $('<strong />').text(strong + ": ");
        var span = $('<span />').text(span);
        li.append(strong);
        li.append(span);
        ul.append(li);
    }

    var allocator_stuff = [
        ['Allocated', 'bytes', 'default_allocator_bytes_allocated'],
        ['Peak size', 'bytes', 'default_allocator_peak_bytes_allocated'],
        ['Num mallocs', '', 'default_allocator_allocation_count'],
    ];

    var ul = this.counts_ul.empty();

    var entries = 0;
    $.each(allocator_stuff, function(contents_i, contents)
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

    var framework_stuff = [
        ['Total msgs', '', 'framework_counter_messages_processed'],
        ['Thread yields', '', 'framework_counter_yields'],
        ['Local pushes', '', 'framework_counter_local_pushes'],
        ['Shared pushes', '', 'framework_counter_shared_pushes'],
        ['Msg queue max', '', 'framework_counter_mailbox_queue_max'],
    ];

    if (point.thread_pools.length != this.tp_infos.length)
    {
        this.make_tabbed_thread_pool_counts(point.thread_pools.length);
    }

    $.each(point.thread_pools, function(tp_i, tp_info)
    {
        $('ul', self.tp_infos[tp_i]).remove(); //clear old lists
        ul = $('<ul />');
        self.tp_infos[tp_i].append(ul);

        $.each(framework_stuff, function(contents_i, contents)
        {
            var dir = contents[0];
            var units = contents[1];
            var key = contents[2];
            var count = (key in tp_info)? tp_info[key] : 0;
            if (count > 0)
            {
                make_entry(dir, count.toString() + ' ' + units);
            }
        });
    });
}
