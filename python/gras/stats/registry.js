/***********************************************************************
 * Per block registry data structure
 **********************************************************************/
var GrasBlockRegistry = function(id)
{
    this.id = id;
    this.enabled = true;
}

/***********************************************************************
 * Stats registry data structure
 **********************************************************************/
var GrasStatsRegistry = function()
{
    this.ids = new Array();
    this.enables = new Array();
    this.history = new Array();
    this.overall_rate = 2.0;
    this.overall_active = true;
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
