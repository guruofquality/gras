function GrasChartOverallThroughput(args)
{
    //save enables
    this.ids = args.block_ids;

    //input checking
    if (this.ids.length <= 1) gras_error_dialog(
        "GrasChartOverallThroughput",
        "Error making overall thoughput chart.\n"+
        "Specify at least 2 blocks for this chart."
    );

    //make new chart
    this.chart = new google.visualization.LineChart(args.panel);

    this.title = "Overall Throughput vs Time";
}

GrasChartOverallThroughput.prototype.update = function(history)
{
    if (history.length < 2) return;

    var data_set = [['Throughput'].concat(this.ids)];
    for (var i = Math.max(history.length-10, 1); i < history.length; i++)
    {
        var row = new Array();
        row.push(i.toString());
        for (var j = 0; j < this.ids.length; j++)
        {
            row.push(gras_extract_throughput_delta(history[i-1], history[i], this.ids[j])/1e6);
        }
        data_set.push(row);
    }

    var chart_data = google.visualization.arrayToDataTable(data_set);
    var options = {
        width:$('#page').width()*0.33,
        chartArea:{left:0,top:0,right:0,bottom:0,width:"100%",height:"85%"},
        legend: {'position': 'bottom'},
    };
    this.chart.draw(chart_data, options);
};
