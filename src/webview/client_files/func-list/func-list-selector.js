//Abstract class to manage to properties to list functions with the selected cost
function MattFuncListSelector(defaults)
{
	//update selecteion
	this.updateMetric = function(name)
	{
		this.selected = name;
		this.onChange();
	}

	//toogle unit
	this.toogleUnit = function()
	{
		if (this.unit == '%')
			this.unit = 'real';
		else
			this.unit = '%';
		this.onChange();
	}
	
	//toogle mode
	this.toogleMode = function()
	{
		if (this.mode == 'total')
			this.mode = 'own';
		else
			this.mode = 'total';
		this.onChange();
	}
	
	//toogle sort
	this.toogleSort = function()
	{
		if (this.sort == 'asc')
			this.sort = 'desc';
		else
			this.sort = 'asc';
		this.onChange();
	}
	
	//register actions for events
	this.onChange = function()
	{
	}
	
	//return the current selector
	this.getSelector = function()
	{
		return this.valueSelector[this.selected];
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
	
	//get ref
	this.internalGetRef = function(max,sum,selector)
	{
		if (selector.ref == 'max' || this.mode == 'total')
			return max;
		else
			return sum;
	}
	
	//get unit 
	this.internalGetUnit = function(selector)
	{
		if (this.unit == 'real')
			return selector.unit;
		else
			return '%';
	}
	
	//Extract function list and format with user requirement
	this.extractData = function(data)
	{
		var max = 0;
		var sum = 0;
		var res = new Array();
		var selector = this.getSelector();
		for(var i in data)
		{
			var value = this.internalGetSelectedValue(data[i],selector);
			if (value != 0)
			{
				if (value > max)
					max = value;
				sum += value;
				var d = data[i];
				res.push({name:d.function,'value':value,'details':d});
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
	
	//check values
	var acceptedModes = ['total','own'];
	var acceptedSort  = ['asc','desc'];
	var acceptedUnit  = ['%','real'];
	this.sanityCheck = function()
	{
		mattHelper.assert($.inArray(this.mode,this.acceptedModes),"Invalid property 'mode' : " + this.mode);
		mattHelper.assert($.inArray(this.sort,this.acceptedSort),"Invalid property 'sort' : " + this.mode);
		mattHelper.assert($.inArray(this.unit,this.acceptedUnit),"Invalid property 'unit' : " + this.unit);
		mattHelper.assert(this.valueSelector[this.selected] != undefined,"Invalid property 'selected' : " + this.selected);
	}
	
	//setup default modes
	this.mode = 'total'; //total | own
	this.selected = 'alloc.sum';//alloc.sum | alloc.count | alloc.min | alloc.max... (see this.valueSelector)
	this.unit = 'real';//real | percent
	this.sort = 'asc';//asc | desc
	
	//apply user selection
	if (defaults != undefined)
	{
		if (defaults.selected != undefined) this.selected = defaults.selected;
		if (defaults.mode != undefined) this.mode = defaults.mode;
		if (defaults.unit != undefined) this.unit = defaults.unit;
		if (defaults.sort != undefined) this.sort = defaults.sort;
	}
		
	
	//build value extractor
	this.valueSelector = new Object();
	this.valueSelector['alloc.sum']   = {name: 'Allocated mem.',extractor:function (entry) {return entry.alloc.sum;},'unit' : 'B',ref:'sum'};
	this.valueSelector['alloc.count'] = {name: 'Allocation num.',extractor:function (entry) {return entry.alloc.count;},'unit':'',ref:'sum'};
	this.valueSelector['alloc.min']   = {name: 'Min. alloc. size',extractor:function (entry) {return entry.alloc.min;},'unit':'B',ref:'max'};
	this.valueSelector['alloc.max']   = {name: 'Max. alloc. size',extractor:function (entry) {return entry.alloc.max;},'unit':'B',ref:'max'};
	this.valueSelector['alloc.moy']   = {name: 'Mean alloc. size',extractor:function (entry) {return entry.alloc.count == 0 ? 0 : entry.alloc.sum / entry.alloc.count;},'unit':'B',ref:'max'};
	this.valueSelector['free.sum']    = {name: 'Freed mem.',extractor:function (entry) {return entry.free.sum;},'unit' : 'B',ref:'sum'};
	this.valueSelector['free.count']  = {name: 'Free num.',extractor:function (entry) {return entry.free.count;},'unit':'',ref:'sum'};
	this.valueSelector['memops']      = {name: 'Memory ops.',extractor:function (entry) {return entry.alloc.count + entry.free.count;},'unit' : '',ref:'sum'};
	this.valueSelector['alivemem']    = {name: 'Alive memory',extractor:function (entry) {return entry.maxAliveReq;},'unit' : 'B',ref:'sum'};
	this.valueSelector['leaks']       = {name: 'Leaks',extractor:function (entry) {return entry.aliveReq;},'unit' : 'B',ref:'sum'};
}
