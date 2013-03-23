/***********************************************************************
 * One time setup
 **********************************************************************/
var gras_chart_factory_setup = function(registry)
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
 * chart factory dispatcher
 **********************************************************************/
var gras_chart_factory_dispatcher = function()
{
    //step 1) get a list of the selected blocks
    var selected_blocks = new Array();
    $.each($('.chart_designer_blocks > :input'), function(index, input)
    {
        var input = $(input);
        if (input.is(':checked'))
        {
            selected_blocks.append(input.attr('name'));
        }
    });

    //step 2) get the type of chart to create
    var chart_type = $('#chart_type_selector').val();

    //step 3) input validations
}

/***********************************************************************
 * chart factory init
 **********************************************************************/
var gras_chart_registry = new Array();
var gras_chart_factory_init = function()
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
        gras_chart_registry[options['key']] = options['factory'];
        $('#chart_type_selector').append('<option value=' + options['key'] + '>' + options['name'] + '</option>');
    });
}
