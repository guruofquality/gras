function GrasChartHandlerBreakdown(args)
{
    //input checking
    if (args.block_ids.length != 1) throw gras_error_dialog(
        "GrasChartHandlerBreakdown",
        "Error making handler breakdown chart.\n"+
        "Specify only one block for this chart."
    );

    //save enable
    this.block_id = args.block_ids[0];

    //make new chart
    this.chart = new google.visualization.PieChart(args.panel);

    this.title = "Handler Breakdown - " + this.block_id;
}

GrasChartHandlerBreakdown.prototype.update = function(point)
{
    var percents = gras_extract_percent_times(point, this.block_id);
    var data = google.visualization.arrayToDataTable([
          ['Task', 'Percent'],
          ['Work prep', percents['prep']],
          ['Work task', percents['work']],
          ['Work post', percents['post']],
          ['Input tasks', percents['input']],
          ['Output tasks', percents['output']],
    ]);

    var options = {
        width:$('#page').width()/5,
        chartArea:{left:5,top:0,right:5,bottom:0,width:"100%",height:"100%"},
    };

    this.chart.draw(data, options);
};
