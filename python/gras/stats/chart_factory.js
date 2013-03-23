/***********************************************************************
 * One time setup
 **********************************************************************/
function gras_chart_factory_setup(registry)
{
    if (registry.history.length != 1) return;
    var id = $('gras_stats:first', registry.history[0]).attr('id');
    $('#top_name').append(' - ' + id);
    $.each(registry.getBlockIds(), function(index, id)
    {
        var div = $('.chart_designer_blocks').get(index%2);
        $(div).append('<label>' + id + '</label>');
        var input = $('<input />').attr({
            type: 'checkbox',
            name: id
        });
        input.attr('checked', false);
        $(div).append(input);
    });
}

/***********************************************************************
 * chart factory registry (filled in init)
 **********************************************************************/
var gras_chart_factory_registry = new Array();
var gras_chart_active_registry = new Array();

/***********************************************************************
 * chart factory dispatcher
 **********************************************************************/
function gras_chart_factory_dispatcher()
{
    //step 1) get a list of the selected blocks
    var selected_blocks = new Array();
    $.each($('.chart_designer_blocks > :input'), function(index, input)
    {
        var input = $(input);
        if (input.is(':checked'))
        {
            selected_blocks.push(input.attr('name'));
        }
    });

    //step 2) get the type of chart to create
    var chart_type = $('#chart_type_selector').val();

    //step 3) create the chart given options

    //create containers
    var chart_box = $('<table />').attr({class:'chart_container'});
    var tr = $('<tr />');
    var td = $('<td />');
    tr.append(td);

    //call into the factory
    var chart = new gras_chart_factory_registry[chart_type]({
        block_ids:selected_blocks,
        panel:td.get(0),
    });

    //setup the title
    var tr_title = $('<tr />');
    var th_title = $('<th />');
    tr_title.append(th_title);
    th_title.text(chart.title);

    //register the chart
    gras_chart_active_registry.push(chart);
    $('#charts_panel').append(chart_box);

    //finish gui building
    chart_box.append(tr_title);
    chart_box.append(tr);
}

/***********************************************************************
 * chart factory init
 **********************************************************************/
function gras_chart_factory_init()
{
    //install callback for chart factory
    $('#chart_factory_button').click(gras_chart_factory_dispatcher);

    //list of all known chart types
    var chart_options = [
        {key:'overhead_compare', name:'Overhead Compare', factory:GrasChartOverheadCompare},
        {key:'overall_throughput', name:'Overall Throughput', factory:GrasChartOverallThroughput},
        {key:'handler_breakdown', name:'Handler Breakdown', factory:GrasChartHandlerBreakdown},
    ];

    //init the chart selection input
    $.each(chart_options, function(index, options)
    {
        gras_chart_factory_registry[options.key] = options.factory;
        var option = $('<option />').attr({value: options.key});
        option.text(options.name);
        $('#chart_type_selector').append(option);
    });
}
