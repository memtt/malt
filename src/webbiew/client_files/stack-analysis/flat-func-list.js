/** Short helper to convert values to human readable format **/
var MATT_POWER = ['&nbsp;','K','M','G','T','P'];
function mattHumanValue(data,value)
{
	if (data.unit == '%')
	{
		return (100.0*value/data.ref).toFixed(1) + " %";
	} else {
		var power = 0;
		while (value >= 1000)
		{
			power++;
			value /= 1000;
		}

		//return value.toFixed(1) + " " + MATT_POWER[power] + data.unit;
		return Math.round(value) + " " + MATT_POWER[power] + data.unit;
	}
}

/** MattFuncList class **/
function MattFuncList(ulId)
{
	//Store ID
	this.ulId = ulId;
	this.ul = document.getElementById(ulId);
	
	//register current object onto object
	this.ul.matt = this;
	
	//setup default modes
	this.mode = 'total'; //total | own
	this.selected = 'alloc.sum';//alloc.sum | alloc.count | alloc.min | alloc.max... (see this.valueSelector)
	this.unit = 'real';//real | percent
	this.sort = 'asc';//asc | desc
	
	//data not fetch now
	this.data = null;
	
	//build value extractor
	this.valueSelector = new Object();
	this.valueSelector['alloc.sum']   = {'name': 'Allocated mem.','extractor':function (entry) {return entry.alloc.sum;},'unit' : 'B','ref':'sum'};
	this.valueSelector['alloc.count'] = {'name': 'Allocations num.','extractor':function (entry) {return entry.alloc.count;},'unit':'','ref':'sum'};
	this.valueSelector['alloc.min'] = {'name': 'Min. alloc. size','extractor':function (entry) {return entry.alloc.min;},'unit':'B','ref':'max'};
	this.valueSelector['alloc.max'] = {'name': 'Max. alloc. size','extractor':function (entry) {return entry.alloc.max;},'unit':'B','ref':'max'};
	this.valueSelector['alloc.moy'] = {'name': 'Mean alloc. size','extractor':function (entry) {return entry.alloc.count == 0 ? 0 : entry.alloc.sum / entry.alloc.count;},'unit':'B','ref':'max'};
	this.valueSelector['free.count'] = {'name': 'Free num.','extractor':function (entry) {return entry.free.count;},'unit':'','ref':'sum'};
	
	//Declare render function
	this.render = function()
	{
		if (this.data == null)
		{
			this.internalFetchAndRender();
		} else {
			this.internalRender();
		}
	}
	
	//update selecteion
	this.updateMetric = function(name)
	{
		this.selected = name;
		this.render();
	}

	//toogle unit
	this.toogleUnit = function()
	{
		if (this.unit == '%')
			this.unit = 'real';
		else
			this.unit = '%';
		this.render();
	}
	
	//toogle mode
	this.toogleMode = function()
	{
		if (this.mode == 'total')
			this.mode = 'own';
		else
			this.mode = 'total';
		this.render();
	}
	
	//toogle sort
	this.toogleSort = function()
	{
		if (this.sort == 'asc')
			this.sort = 'desc';
		else
			this.sort = 'asc';
		this.render();
	}
	
	//Declare intenral fetchAndRender
	this.internalFetchAndRender = function()
	{
		var mattObj = this;
		$.getJSON( "flat.json", function( data ) {
			mattObj.data = data;
			mattObj.internalRender();
		});
	}
	
	this.renderSelectorList = function(id)
	{
		var callback = new EJS({url: '/stack-analysis/flat-func-selector.ejs'}).update(id);
		callback({'selectors':this.valueSelector,'listId':this.ulId});
	}

	//Declare internal render
	this.internalRender = function()
	{
		this.internalSanityCheck();
		var callback = new EJS({url: '/stack-analysis/flat-func-list.ejs'}).update(this.ulId);
		callback(this.intenralExtractFunctionList(this.data));
	}
	
	//extract selected value from data entry
	this.internalGetSelectedValue = function(entry,selector)
	{
		var selMode;
		if (this.mode == 'total')
			selMode = entry.total;
		else if (this.mode == 'own')
			selMode = entry.own;
		
		if (selMode == undefined)
			return 0;
		
		return selector.extractor(selMode);
	}
	
	//get unit 
	this.internalGetUnit = function(selector)
	{
		if (this.unit == 'real')
			return selector.unit;
		else
			return '%';
	}
	
	//get ref
	this.internalGetRef = function(max,sum,selector)
	{
		if (selector.ref == 'max' || this.mode == 'total')
			return max;
		else
			return sum;
	}
	
	//check values
	var acceptedModes = ['total','own'];
	var acceptedSort  = ['asc','desc'];
	var acceptedUnit  = ['%','real'];
	this.internalSanityCheck = function()
	{
		mattHelper.assert($.inArray(this.mode,this.acceptedModes),"Invalid property 'mode' : " + this.mode);
		mattHelper.assert($.inArray(this.sort,this.acceptedSort),"Invalid property 'sort' : " + this.mode);
		mattHelper.assert($.inArray(this.unit,this.acceptedUnit),"Invalid property 'unit' : " + this.unit);
		mattHelper.assert(this.valueSelector[this.selected] != undefined,"Invalid property 'selected' : " + this.selected);
	}
	
	//Extract function list and format with user requirement
	this.intenralExtractFunctionList = function(data)
	{
		var max = 0;
		var sum = 0;
		var res = new Array();
		var selector = this.valueSelector[this.selected];
		for(var i in data)
		{
			var value = this.internalGetSelectedValue(data[i],selector);
			if (value != 0)
			{
				if (value > max)
					max = value;
				sum += value;
				res.push({'name':i,'value':value,'details':data[i]});
			}
		}

		//sort
		if (this.sort == 'asc')
			res = res.sort(function (a,b) {return (b.value - a.value)});
		else
			res = res.sort(function (a,b) {return (a.value - b.value)});

		//ok return for render
		return {'data':{'entries':res, 'ref':this.internalGetRef(max,sum,selector),'unit':this.internalGetUnit(selector)}};
	}
	
	//do render by default
	this.render();
}
