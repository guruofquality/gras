function GrasChartOverallThroughput(args, panel)
{
    //save enables
    this.ids = args.block_ids;

    //input checking
    if (this.ids.length == 0) throw gras_error_dialog(
        "GrasChartOverallThroughput",
        "Error making overall thoughput chart.\n"+
        "Specify at least 1 block for this chart."
    );

    //make new chart
    this.chart = new google.visualization.LineChart(panel);

    this.title = "Overall Throughput vs Time in MIps";
    this.history = new Array();
    this.default_width = 2*GRAS_CHARTS_STD_WIDTH;
}

GrasChartOverallThroughput.prototype.update = function(point)
{
    this.history.push(point);
    if (this.history.length == 1) this.p0 = point;
    if (this.history.length < 2) return;
    if (this.history.length > 10) this.history.splice(0, 1);

    var data_set = [['Throughput'].concat(this.ids)];
    for (var i = 1; i < this.history.length; i++)
    {
        var row = new Array();
        row.push(gras_extract_stat_time_delta(this.p0, this.history[i]).toFixed(2).toString());
        for (var j = 0; j < this.ids.length; j++)
        {
            row.push(gras_extract_throughput_delta(this.history[i-1], this.history[i], this.ids[j])/1e6);
        }
        data_set.push(row);
    }

    var chart_data = google.visualization.arrayToDataTable(data_set);
    var options = {
        legend: {'position': 'bottom'},
    };
    if (this.gc_resize) options.width = 50;
    if (this.gc_resize) options.height = 50;
    this.chart.draw(chart_data, options);
};
