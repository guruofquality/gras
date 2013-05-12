function GrasChartPortDowntime(args, panel)
{
    //input checking
    if (args.block_ids.length != 1) throw gras_error_dialog(
        "GrasChartPortDowntime",
        "Error making port downtime chart.\n"+
        "Specify only one block for this chart."
    );

    //save enable
    this.block_id = args.block_ids[0];

    //make new chart
    this.chart = new google.visualization.PieChart(panel);

    this.title = "Port Downtime - " + this.block_id;
    this.default_width = GRAS_CHARTS_STD_WIDTH;
}

GrasChartPortDowntime.prototype.update = function(point)
{
    var block_data = point.blocks[this.block_id];
    if (!block_data) return;

    var raw_data = new Array();
    raw_data.push(['Port', 'Percent']); //key

    //now add input and output port data
    $.each(block_data.inputs_idle, function(index, downtime)
    {
        raw_data.push(['Input'+index.toString(), downtime/block_data.tps]);
    });
    $.each(block_data.outputs_idle, function(index, downtime)
    {
        raw_data.push(['Output'+index.toString(), downtime/block_data.tps]);
    });

    //update the chart from raw data
    var data = google.visualization.arrayToDataTable(raw_data);
    var options = {
        chartArea:{left:5,top:0,right:5,bottom:0,width:"100%",height:"100%"},
    };
    if (this.gc_resize) options.width = 50;
    if (this.gc_resize) options.height = 50;

    this.chart.draw(data, options);
};
