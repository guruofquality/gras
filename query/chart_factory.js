/***********************************************************************
 * Some constants
 **********************************************************************/
var GRAS_CHARTS_STD_WIDTH = 250;

/***********************************************************************
 * Chart registry for now chart types
 **********************************************************************/
var gras_chart_get_registry = function()
{
    return [
        {key:'overhead_compare', name:'Overhead Compare', factory:GrasChartOverheadCompare},
        {key:'overall_throughput', name:'Overall Throughput', factory:GrasChartOverallThroughput},
        {key:'handler_breakdown', name:'Handler Breakdown', factory:GrasChartHandlerBreakdown},
        {key:'port_counters', name:'Port Counters', factory:GrasChartPortCounts},
        {key:'global_counters', name:'Global Counters', factory:GrasChartGlobalCounts},
        {key:'port_downtime', name:'Port downtime', factory:GrasChartPortDowntime},
        {key:'topology_display', name:'Topology Display', factory:GrasChartTopologyDisplay},
    ];
}

/***********************************************************************
 * get blocks that need active querying
 **********************************************************************/
function gras_chart_factory_active_blocks(registry)
{
    var block_ids = new Array();
    $.each(registry.active_charts, function(index, chart_info)
    {
        $.merge(block_ids, chart_info.args.block_ids);
    });
    return $.unique(block_ids);
}

/***********************************************************************
 * update after new query event
 **********************************************************************/
function gras_chart_factory_update(registry, point)
{
    registry.point = point; //store last data point
    $.each(registry.active_charts, function(index, chart_info)
    {
        try
        {
            chart_info.chart.update(point);
        }
        catch(err)
        {
            //jQuery.error("gras_chart_factory_update error: " + err.message);
        }
    });
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
    localStorage.setItem(registry.top_id, JSON.stringify({
        chart_args: all_args,
        overall_rate: registry.overall_rate,
        overall_active: registry.overall_active,
    }));
}

function gras_chart_load(registry)
{
    if (typeof(Storage) === "undefined") return;
    var storage = JSON.parse(localStorage.getItem(registry.top_id));
    if (!storage) return;

    //restore misc settings in storage
    registry.overall_rate = storage.overall_rate;
    registry.overall_active = storage.overall_active;

    //rebuild all charts from args
    $.each(storage.chart_args, function(args_i, args)
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
    try
    {
        var chart = new registry.chart_factories[args.chart_type](args, td.get(0));
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
    var chart_info = {chart:chart,args:args,panel:chart_box};
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

    //implement draggable and resizable from jquery ui
    var handle_stop = function(event, ui)
    {
        args['width'] = chart_box.width();
        args['height'] = chart_box.height();
        args['position'] = chart_box.offset();
        chart.gc_resize = false;
        chart.update(registry.point);
        gras_chart_save(registry);
    };

    if ('default_width' in chart) chart_box.width(chart.default_width);
    chart_box.resizable({stop: handle_stop, create: function(event, ui)
    {
        if ('width' in args) chart_box.width(args.width);
        if ('height' in args) chart_box.height(args.height);
    },
    start: function(event, ui)
    {
        chart.gc_resize = true;
        chart.update(registry.point);
    }});

    chart_box.css('position', 'absolute');
    chart_box.draggable({stop: handle_stop, create: function(event, ui)
    {
        if ('position' in args) chart_box.offset(args.position);
    }, cursor: "move"});

    //set the cursor on the title bar so its obvious
    tr_title.hover(
        function(){$(this).css('cursor','move'); close_div.show();},
        function(){$(this).css('cursor','auto'); close_div.hide();}
    );
    close_div.hide();
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

    //init chart overall gui controls
    var overall_rate = $('#chart_update_rate').attr({size:3});
    overall_rate.spinner({
        min: 1, max: 10, step: 0.5, stop: function(event, ui){$(this).change();}
    });
    var overall_active = $('#chart_active_state');
    overall_active.button();

    //callback for overall gui events
    function handle_gui_event()
    {
        registry.overall_active = overall_active.is(':checked');
        registry.overall_rate = overall_rate.val();
        gras_chart_save(registry);
    }
    overall_rate.change(handle_gui_event);
    overall_active.change(handle_gui_event);

    //block registry and checkboxes init
    $.getJSON('/blocks.json', function(data)
    {
        var container = $('#chart_designer_blocks');
        $.each(data.blocks, function(id, attrs)
        {
            registry.block_ids.push(id);
            var cb_id = "chart_designer_blocks " + id;
            var div = $('<div />');
            var label = $('<label />').text(id).attr({'for':cb_id});
            var input = $('<input />').attr({
                type: 'checkbox',
                name: id,
                id: cb_id,
            });
            input.attr('checked', false);
            div.append(input);
            div.append(label);
            container.append(div);
        });
        //container.buttonset();

        //try to load last settings
        try{gras_chart_load(registry);}catch(e){}

        //init gui elements after settings restore
        overall_rate.val(registry.overall_rate);
        overall_active.attr('checked', registry.overall_active);
        handle_gui_event();
        gras_query_stats(registry);
    });
}
