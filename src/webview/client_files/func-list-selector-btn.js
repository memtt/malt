/********************************************************************/
/**
 * Build and manage the buttons to manipulate the func list selector state.
**/
function MattFuncListSelectorButtons(containerId,defaults)
{
	//check
	mattHelper.assert(containerId != undefined);
	
	//setup
	this.selector = new MattFuncListSelector(defaults);
	this.containerId = containerId;
	this.container = document.getElementById(containerId);
	
	//check and links to find this object on button actions
	mattHelper.assert(this.container != undefined);
	this.container.matt = this;
	
	//final rendering
	this.render();
}

/********************************************************************/
MattFuncListSelectorButtons.prototype.render = function()
{
	var callback = new EJS({url: 'func-list-selector-btn-view.ejs'}).update(this.containerId);
	callback({metrics:this.selector.getMetrics(),divId:this.containerId});
}

/********************************************************************/
MattFuncListSelectorButtons.prototype.getSelector = function()
{
	return this.selector;
}

/********************************************************************/
MattFuncListSelectorButtons.prototype.updateMetric = function(name)
{
	this.selector.updateMetric(name);
	document.getElementById(this.containerId + '.button-metric').innerHTML = this.selector.getMetric().name;
}

/********************************************************************/
MattFuncListSelectorButtons.prototype.toogleUnit = function()
{
	this.selector.toogleUnit();
}

/********************************************************************/
MattFuncListSelectorButtons.prototype.toogleMode = function()
{
	this.selector.toogleMode();
}

/********************************************************************/
MattFuncListSelectorButtons.prototype.toogleSort = function()
{
	this.selector.toogleSort();
}

/********************************************************************/
MattFuncListSelectorButtons.prototype.moveNextPage = function()
{
	this.selector.moveNextPage();
}

/********************************************************************/
MattFuncListSelectorButtons.prototype.movePrevPage = function()
{
	this.selector.movePrevPage();
}

/********************************************************************/
MattFuncListSelectorButtons.prototype.setFilter = function(value)
{
	this.selector.setFilter(value);
}