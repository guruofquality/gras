/***********************************************************************
 * Stats registry data structure
 **********************************************************************/
var GrasStatsRegistry = function()
{
    this.ids = new Array();
    this.enables = new Array();
    this.history = new Array();
}

GrasStatsRegistry.prototype.appendPoint = function(point)
{
    this.history.push(point);
    var self = this;
    if (this.history.length == 1)
    {
        $('block', this.history[0]).each(function()
        {
            self.ids.push($(this).attr('id'));
        });
    }
};

GrasStatsRegistry.prototype.getBlockIds = function()
{
    var ids = new Array();
    $('block', this.history[0]).each(function()
    {
        ids.push($(this).attr('id'));
    });
    return ids;
};
