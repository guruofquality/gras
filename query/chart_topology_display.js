function GrasChartTopologyDisplay(args, panel)
{
    //input checking

    //settings
    this.div = $('<div />');
    $(panel).append(this.div);
    this.title = "Topology Display";

    //fetch the topology image given the blocks
    var img = new Image();
    img.src = "/topology.dot.png?" + jQuery.param({blocks:args.block_ids}, true /*needed to parse data*/);
    this.div.append($(img));
}

GrasChartTopologyDisplay.prototype.update = function(point)
{
    var self = this;
}
