function mattAssert(condition,message)
{
	if (!condition)
		throw Error("Assert failed" + (typeof message !== "undefined" ? ": " + message : ""));
}

/** Short helper to convert values to human readable format **/
var MATT_POWER = ['&nbsp;','K','M','G','T','P'];
function mattHumanValue(data,value)
{
	if (data.unit == '%')
	{
		return (100.0*value/data.ref).toFixed(1) + " %";
	} else {
		var power = 0;
		while (value >= 100)
		{
			power++;
			value /= 1000;
		}

		return value.toFixed(1) + " " + MATT_POWER[power] + data.unit;
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
	this.select = 'alloc.sum';//alloc.sum | alloc.count | alloc.min | alloc.max
	this.unit = 'real';//real | percent
	this.sort = 'asc';//asc | desc
	
	//data not fetch now
	this.data = null;
	
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
	
	//Declare internal render
	this.internalRender = function()
	{
		this.internalSanityCheck();
		var callback = new EJS({url: 'func-list.ejs'}).update(this.ulId);
		callback(this.intenralExtractFunctionList(this.data));
	}
	
	//extract selected value from data entry
	this.internalGetSelectedValue = function(entry)
	{
		var selMode;
		if (this.mode == 'total')
			selMode = entry.total;
		else if (this.mode == 'own')
			selMode = entry.own;
		
		if (selMode == undefined)
			return 0;
		
		return selMode.alloc.sum;
	}
	
	//get unit 
	this.internalGetUnit = function()
	{
		if (this.unit == 'real')
			return 'B';
		else
			return '%';
	}
	
	//get ref
	this.internalGetRef = function(max,sum)
	{
		return sum;
	}
	
	//check values
	var acceptedModes = ['total','own'];
	var acceptedSort  = ['asc','desc'];
	var acceptedUnit  = ['%','real'];
	this.internalSanityCheck = function()
	{
		mattAssert($.inArray(this.mode,this.acceptedModes),"Invalid property 'mode' : " + this.mode);
		mattAssert($.inArray(this.sort,this.acceptedSort),"Invalid property 'sort' : " + this.mode);
		mattAssert($.inArray(this.unit,this.acceptedUnit),"Invalid property 'unit' : " + this.unit);
	}
	
	//Extract function list and format with user requirement
	this.intenralExtractFunctionList = function(data)
	{
		var max = 0;
		var sum = 0;
		var res = new Array();
		for(var i in data.funcs)
		{
			var value = this.internalGetSelectedValue(data.funcs[i]);
			if (value > max)
				max = value;
			sum += value;
			res.push({'name':i,'value':value,'details':data.funcs[i]});
		}

		//sort
		if (this.sort == 'asc')
			res = res.sort(function (a,b) {return (b.value - a.value)});
		else
			res = res.sort(function (a,b) {return (a.value - b.value)});

		//ok return for render
		return {'data':{'entries':res, 'ref':this.internalGetRef(max,sum),'unit':this.internalGetUnit()}};
	}
	
	//do render by default
	this.render();
}
