/***********************************************************************
 * Chart registry for now chart types
 **********************************************************************/
var gras_chart_get_registry = function()
{
    return [
        {key:'overhead_compare', name:'Overhead Compare', factory:GrasChartOverheadCompare},
        {key:'overall_throughput', name:'Overall Throughput', factory:GrasChartOverallThroughput},
        {key:'handler_breakdown', name:'Handler Breakdown', factory:GrasChartHandlerBreakdown},
        {key:'total_io_counts', name:'I/O port Totals', factory:GrasChartTotalIoCounts},
    ];
}

/***********************************************************************
 * One time setup
 **********************************************************************/
function gras_chart_factory_setup(registry, point)
{
    //gui init for factory controls
    gras_chart_factory_init(registry);

    //block registry and checkboxes init
    $.each(point.blocks, function(id, block)
    {
        registry.block_ids.push(id);
        var container = $('#chart_designer_blocks');
        var div = $('<div />');
        $(div).append('<label>' + id + '</label>');
        var input = $('<input />').attr({
            type: 'checkbox',
            name: id
        });
        input.attr('checked', false);
        $(div).append(input);
        $(container).append(div);
    });

    //try to load last settings
    try{gras_chart_load(registry);}catch(e){}
}

/***********************************************************************
 * chart factory input handler
 **********************************************************************/
function gras_chart_factory_handle_input(registry)
{
    //get a list of the selected blocks
    var selected_blocks = new Array();
    $.each($('#chart_designer_blocks input'), function(index, input)
    {
        var input = $(input);
        if (input.is(':checked'))
        {
            selected_blocks.push(input.attr('name'));
        }
    });

    //get the type of chart to create
    var chart_type = $('#chart_type_selector').val();

    //create args for the factory make
    var args = {
        block_ids:selected_blocks,
        chart_type:chart_type,
    };

    //call into the factory with args
    gras_chart_factory_make(registry, args);
}

/***********************************************************************
 * save/load to/from local storage
 **********************************************************************/
function gras_chart_save(registry)
{
    if (typeof(Storage) === "undefined") return;
    var all_args = new Array();
    $.each(registry.active_charts, function(index, info)
    {
        all_args.push(info.args);
    });
    localStorage.setItem(registry.top_id, JSON.stringify(all_args));
}

function gras_chart_load(registry)
{
    if (typeof(Storage) === "undefined") return;
    var chart_args = JSON.parse(localStorage.getItem(registry.top_id));
    if (!chart_args) return;
    $.each(chart_args, function(args_i, args)
    {
        //check that the blocks saved in the args actually exist
        var do_make = true;
        $.each(args.block_ids, function(block_id_i, block_id)
        {
            if ($.inArray(block_id, registry.block_ids) < 0)
            {
                do_make = false;
            }
        });
        if (do_make) gras_chart_factory_make(registry, args);
    });
}

/***********************************************************************
 * chart factory make routine
 **********************************************************************/
function gras_chart_factory_make(registry, args)
{
    //create containers
    var chart_box = $('<table />').attr({class:'chart_container'});
    var tr = $('<tr />');
    var td = $('<td />');
    tr.append(td);

    //call into the factory
    args.panel = td.get(0);
    try
    {
        var chart = new registry.chart_factories[args.chart_type](args);
    }
    catch(err)
    {
        return;
    }

    //setup the title
    var tr_title = $('<tr />');
    var th_title = $('<th />');
    tr_title.append(th_title);
    th_title.text(chart.title);

    //register the chart
    var chart_info = {chart:chart,args:args};
    registry.active_charts.push(chart_info);
    $('#charts_panel').append(chart_box);

    //close button
    var close_div = $('<div/>').attr({class:'chart_designer_block_close'});
    var close_href = $('<a />').attr({href:'#', class:"ui-dialog-titlebar-close ui-corner-all", role:"button"});
    var close_span = $('<span />').attr({class:"ui-icon ui-icon-closethick"}).text('close');
    close_div.append(close_href);
    close_href.append(close_span);
    th_title.append(close_div);
    $(close_href).click(function()
    {
        var index = $.inArray(chart_info, registry.active_charts);
        registry.active_charts.splice(index, 1);
        chart_box.remove();
        gras_chart_save(registry);
    });
    gras_chart_save(registry);

    //finish gui building
    chart_box.append(tr_title);
    chart_box.append(tr);
}

/***********************************************************************
 * chart factory handle online/offline
 **********************************************************************/
function gras_chart_factory_online(registry)
{
    if (!registry.online) registry.offline_count++;
    if (registry.online) $('#page').css('background-color', '#EEEEFF');
    else if (registry.offline_count%2 == 0) $('#page').css('background-color', '#FF4848');
    else if (registry.offline_count%2 == 1) $('#page').css('background-color', '#EEEEFF');
}

/***********************************************************************
 * chart factory init
 **********************************************************************/
function gras_chart_factory_init(registry)
{
    //init registry containers
    registry.active_charts = new Array();
    registry.chart_factories = new Array();

    //install callback for chart factory
    $('#chart_factory_button').click(function()
    {
        gras_chart_factory_handle_input(registry);
    });

    //init the chart selection input
    $.each(gras_chart_get_registry(), function(index, options)
    {
        registry.chart_factories[options.key] = options.factory;
        var option = $('<option />').attr({value: options.key});
        option.text(options.name);
        $('#chart_type_selector').append(option);
    });

    //init overall config gui element for rate
    var overall_rate = $('#chart_update_rate').attr({size:3});
    overall_rate.spinner({
        min: 1, max: 10, stop: function(event, ui){$(this).change();}
    });
    overall_rate.val(registry.overall_rate);
    overall_rate.change(function()
    {
        registry.overall_rate = overall_rate.val();
    });

    //init overall config gui element for activity
    registry.overall_active = true;
    var overall_active = $('#chart_active_state');
    overall_active.attr('checked', registry.overall_active);
    overall_active.change(function()
    {
        registry.overall_active = overall_active.is(':checked');
        if (registry.overall_active) gras_query_stats(registry);
        else window.clearInterval(registry.timeout_handle);
    });
}
