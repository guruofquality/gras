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
    alert('foo');
}
