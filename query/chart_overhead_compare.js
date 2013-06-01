function GrasChartOverheadCompare(args, panel)
{
    //save enables
    this.ids = args.block_ids;

    //input checking
    if (this.ids.length <= 1) throw gras_error_dialog(
        "GrasChartOverheadCompare",
        "Error making overhead compare chart.\n"+
        "Specify at least 2 blocks for this chart."
    );

    //make new chart
    this.chart = new google.visualization.PieChart(panel);

    this.title = "Overhead Comparison";
    this.default_width = GRAS_CHARTS_STD_WIDTH;
}

GrasChartOverheadCompare.prototype.update = function(point)
{
    var data_set = new Array();
    data_set.push(['Task', 'Percent']);
    $.each(this.ids, function(index, id)
    {
        if (id in point.blocks) //skip this ID if it didnt show up in the data
        {
            var percents = gras_extract_percent_times(point, id);
            data_set.push([id, percents['total']]);
        }
    });

    var data = google.visualization.arrayToDataTable(data_set)

    var options = {
        chartArea:{left:5,top:0,right:5,bottom:0,width:"100%",height:"100%"},
    };
    if (this.gc_resize) options.width = 50;
    if (this.gc_resize) options.height = 50;

    this.chart.draw(data, options);
};
