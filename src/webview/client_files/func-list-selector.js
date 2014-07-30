/********************************************************************/
/**
 * Abstract class to manage to properties to list functions with the selected cost
 * @param defaults Optionnaly provide the default parameters. You can set :
 *     - unit (% | real)
 *     - mode (total | own)
 *     - sort (asc | desc)
 *     - metric (see defined list in constructor)
**/
function MattFuncListSelector(defaults)
{
	//check values
	var acceptedModes = ['total','own'];
	var acceptedSort  = ['asc','desc'];
	var acceptedUnit  = ['%','real'];
	
	//setup default modes
	this.mode = 'total'; //total | own
	this.metricName = 'alloc.sum';//alloc.sum | alloc.count | alloc.min | alloc.max... (see this.metrics)
	this.unit = 'real';//real | percent
	this.sort = 'asc';//asc | desc
	this.from = 0;
	this.count = 4;
	this.filter = '';
	
	//apply user selection
	if (defaults != undefined)
	{
		if (defaults.metric != undefined) this.metricName = defaults.metric;
		if (defaults.mode != undefined) this.mode = defaults.mode;
		if (defaults.unit != undefined) this.unit = defaults.unit;
		if (defaults.sort != undefined) this.sort = defaults.sort;
		if (defaults.from != undefined) this.from = defaults.from;
		if (defaults.count != undefined) this.count = defaults.count;
	}
		
	
	//build value extractor
	this.metrics = new Object();
	this.metrics['alloc.sum']   = {name: 'Allocated mem.',extractor:function (entry) {return entry.alloc.sum;},'unit' : 'B',ref:'sum'};
	this.metrics['alloc.count'] = {name: 'Allocation count',extractor:function (entry) {return entry.alloc.count;},'unit':'',ref:'sum'};
	this.metrics['alloc.min']   = {name: 'Min. size',extractor:function (entry) {return entry.alloc.min;},'unit':'B',ref:'max'};
	this.metrics['alloc.max']   = {name: 'Max. size',extractor:function (entry) {return entry.alloc.max;},'unit':'B',ref:'max'};
	this.metrics['alloc.moy']   = {name: 'Mean size',extractor:function (entry) {return entry.alloc.count == 0 ? 0 : entry.alloc.sum / entry.alloc.count;},'unit':'B',ref:'max'};
	this.metrics['free.sum']    = {name: 'Freed mem.',extractor:function (entry) {return entry.free.sum;},'unit' : 'B',ref:'sum'};
	this.metrics['free.count']  = {name: 'Free count',extractor:function (entry) {return entry.free.count;},'unit':'',ref:'sum'};
	this.metrics['memops']      = {name: 'Memory ops.',extractor:function (entry) {return entry.alloc.count + entry.free.count;},'unit' : '',ref:'sum'};
	this.metrics['lpeakmem']     = {name: 'Local peak',extractor:function (entry) {return entry.maxAliveReq;},'unit' : 'B',ref:'max'};
	this.metrics['gpeakmem']     = {name: 'Global peak',extractor:function (entry) {return entry.globalPeak;},'unit' : 'B',ref:'sum'};
	this.metrics['leaks']       = {name: 'Leaks',extractor:function (entry) {return entry.aliveReq;},'unit' : 'B',ref:'sum'};
	this.metrics['lifetime.max']    = {name: 'Max lifetime',extractor:function (entry) {return entry.lifetime.max;},'unit' : '',ref:'max'};
	this.metrics['lifetime.min']    = {name: 'Min lifetime',extractor:function (entry) {return entry.lifetime.min;},'unit' : '',ref:'max'};
// 	this.metrics['ratio']       = {name: 'Recycling ratio',extractor:function (entry) {return entry.alloc.sum/entry.lifetime.count;},'unit' : 'c',ref:'max'};
	
	//select default metric
	this.metric = this.metrics[this.metricName];
}

/********************************************************************/
/**
 * Return the list of available metrics
**/
MattFuncListSelector.prototype.getMetrics = function()
{
	return this.metrics;
}

/********************************************************************/
/**
 * Checker function to validate the current state of the selector.
**/
MattFuncListSelector.prototype.sanityCheck = function()
{
	mattHelper.assert($.inArray(this.mode,this.acceptedModes),"Invalid property 'mode' : " + this.mode);
	mattHelper.assert($.inArray(this.sort,this.acceptedSort),"Invalid property 'sort' : " + this.mode);
	mattHelper.assert($.inArray(this.unit,this.acceptedUnit),"Invalid property 'unit' : " + this.unit);
	mattHelper.assert(this.metrics[this.metricName] != undefined,"Invalid property 'metricName' : " + this.metricName);
}

/********************************************************************/
/**
 * Select the next part of the list
**/
MattFuncListSelector.prototype.moveNextPage = function()
{
	this.from += this.count;
	this.onChange();
}

/********************************************************************/
/**
 * Select the next part of the list
**/
MattFuncListSelector.prototype.movePrevPage = function()
{
	this.from -= this.count;
	if (this.from < 0)
		this.from = 0;
	this.onChange();
}

/********************************************************************/
MattFuncListSelector.prototype.getRange = function()
{
	return {from:this.from,count:this.count,to:this.from+this.count};
}

/********************************************************************/
/**
 * Update the current selected metric and nofity the onChange event.
**/
MattFuncListSelector.prototype.updateMetric = function(name)
{
	this.metricName = name;
	this.metric = this.metrics[this.metricName];
	mattHelper.assert(this.metric != undefined,"Invalid property 'metricName' : " + this.metricName);
	this.onChange();
}

/********************************************************************/
/**
 * Toogle unit from % to real and notify with onChange event.
**/
MattFuncListSelector.prototype.toogleUnit = function()
{
	if (this.unit == '%')
		this.unit = 'real';
	else
		this.unit = '%';
	this.onChange();
}

/********************************************************************/
/**
 * Toogle between 'own' and 'total' mode then notify with onChange event.
**/
MattFuncListSelector.prototype.toogleMode = function()
{
	if (this.mode == 'total')
		this.mode = 'own';
	else
		this.mode = 'total';
	this.onChange();
}

/********************************************************************/
/**
 * Toogle the sorting mode and notify with onChange event.
**/
MattFuncListSelector.prototype.toogleSort = function()
{
	if (this.sort == 'asc')
		this.sort = 'desc';
	else
		this.sort = 'asc';
	this.onChange();
}

/********************************************************************/
/**
 * To be extended by user to implement its own actions to update views.
**/
MattFuncListSelector.prototype.onChange = function()
{
}

/********************************************************************/
/**
 * return the current selector
**/
MattFuncListSelector.prototype.getMetric = function()
{
	return this.metrics[this.metricName];
}

/********************************************************************/
/**
 * extract selected value from data entry depending on the mode (total/own) and the metric
**/
MattFuncListSelector.prototype.internalGetSelectedValue = function(entry)
{
	var selMode;
	if (this.mode == 'total')
		selMode = entry.total;
	else if (this.mode == 'own')
		selMode = entry.own;
	
	if (selMode == undefined)
		return 0;
	
	return this.metric.extractor(selMode);
}

/********************************************************************/
/**
 * Return the ref value (to compute %) depending on the mode and the metric.
**/
MattFuncListSelector.prototype.internalGetRef = function(max,sum)
{
	if (this.metric.ref == 'max' || this.mode == 'total')
		return max;
	else
		return sum;
}

/********************************************************************/
/**
 * get unit of current metric
**/
MattFuncListSelector.prototype.internalGetUnit = function()
{
	if (this.unit == 'real')
		return this.metric.unit;
	else
		return '%';
}

/********************************************************************/
MattFuncListSelector.prototype.setFilter = function(value)
{
	this.filter = value;
	this.onChange();
}

/********************************************************************/
/**
 * Extract function list and format with user requirement
**/
MattFuncListSelector.prototype.extractData = function(data)
{
	var max = 0;
	var sum = 0;
	var res = new Array();

	for(var i in data)
	{
		var d = data[i];
		if (this.filter == '' || d.function.indexOf(this.filter) > -1)
		{
			var value = this.internalGetSelectedValue(d);
			if (value != 0)
			{
				if (value > max)
					max = value;
				sum += value;
				res.push({name:d.function,'value':value,'details':d});
			}
		}
	}

	//sort
	if (this.sort == 'asc')
		res = res.sort(function (a,b) {return (b.value - a.value)});
	else
		res = res.sort(function (a,b) {return (a.value - b.value)});

	//ok return for render
	return {'entries':res, 'ref':this.internalGetRef(max,sum),'unit':this.internalGetUnit()};
}
