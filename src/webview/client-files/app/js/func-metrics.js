//declare metrics
var maltMetrics = {
	'alloc.sum': {
		name: 'Allocated mem.',
		extractor: function(x) {return x.alloc.sum;},
		formalter: function(x) {return maltHelper.humanReadable(x,1,'B',false);},
		defaultOrder: 'desc',
		ref: 'sum'
	},
	'alloc.count': {
		name: 'Allocated count',
		extractor: function(x) {return x.alloc.count;},
		formalter: function(x) {return maltHelper.humanReadable(x,1,'',false);},
		defaultOrder: 'desc',
		ref: 'sum'
	},
	'alloc.min': {
		name: 'Min. alloc size',
		extractor: function(x) {return x.alloc.min;},
		formalter: function(x) {return maltHelper.humanReadable(x,1,'B',false);},
		defaultOrder: 'asc',
		ref: 'max'
	},
	'alloc.mean': {
		name: 'Mean alloc size',
		extractor: function(x) {return x.alloc.count == 0 ? 0 : x.alloc.sum / x.alloc.count;},
		formalter: function(x) {return maltHelper.humanReadable(x,1,'B',false);},
		defaultOrder: 'asc',
		ref: 'max'
	},
	'alloc.max': {
		name: 'Max. alloc size',
		extractor: function(x) {return x.alloc.max;},
		formalter: function(x) {return maltHelper.humanReadable(x,1,'B',false);},
		defaultOrder: 'desc',
		ref: 'max'
	},
	'free.sum': {
		name: 'Freed mem.',
		extractor: function(x) {return x.free.sum;},
		formalter: function(x) {return maltHelper.humanReadable(x,1,'B',false);},
		defaultOrder: 'desc',
		ref: 'sum'
	},
	'free.count': {
		name: 'Free count',
		extractor: function(x) {return x.free.count;},
		formalter: function(x) {return maltHelper.humanReadable(x,1,'',false);},
		defaultOrder: 'desc',
		ref: 'sum'
	},
	'memops': {
		name: 'Memory ops.',
		extractor: function(x) {return x.alloc.count + x.free.count;},
		formalter: function(x) {return maltHelper.humanReadable(x,1,'',false);},
		defaultOrder: 'desc',
		ref: 'sum'
	},
	'peakmem.local': {
		name: 'Local peak',
		extractor: function(x) {return x.maxAliveReq;},
		formalter: function(x) {return maltHelper.humanReadable(x,1,'B',false);},
		defaultOrder: 'desc',
		ref: 'max'
	},
	'peakmem.global': {
		name: 'Global peak',
		extractor: function(x) {return x.globalPeak;},
		formalter: function(x) {return maltHelper.humanReadable(x,1,'B',false);},
		defaultOrder: 'desc',
		ref: 'sum'
	},
	'leaks': {
		name: 'Leaks',
		extractor: function(x) {return x.aliveReq;},
		formalter: function(x) {return maltHelper.humanReadable(x,1,'B',false);},
		defaultOrder: 'desc',
		ref: 'sum'
	},
	'lifetime.max': {
		name: 'Max lifetime',
		extractor: function(x) {return x.lifetime.max;},
		formalter: function(x) {return maltHelper.humanReadable(x,1,'',false);},
		defaultOrder: 'desc',
		ref: 'max'
	},
	'lifetime.min': {
		name: 'Min lifetime',
		extractor: function(x) {return x.lifetime.min;},
		formalter: function(x) {return maltHelper.humanReadable(x,1,'',false);},
		defaultOrder: 'asc',
		ref: 'max'
	},
	'recycling.ratio': {
		name: 'Recycling ratio',
		extractor: function(x) {return x.alloc.count == 0 ? 1 : x.alloc.sum / x.alloc.maxAliveReq;},
		formalter: function(x) {return maltHelper.humanReadable(x,1,'',false);},
		defaultOrder: 'desc',
		ref: 'max'
	},
};

function MattFuncMetrics()
{
}

MattFuncMetrics.prototype.getMetricList = function()
{
	var ret = [];
	for (var i in maltMetrics)
	{
		ret.push({name:maltMetrics[i].name,key:i});
	}
	return ret;
}

MattFuncMetrics.prototype.getMetricNames = function()
{
	var res = [];
	for (var i in maltMetrics)
		res.push(maltMetrics[i].name);
	return res;
}

MattFuncMetrics.prototype.getValue = function(dataElement,metricName,inclusive)
{
	var metric = maltMetrics[metricName];

	if (dataElement == undefined)
	{
		return 0;
	} else if (inclusive) {
		return metric.extractor(dataElement.total);
	} else if (dataElement.own == undefined) {
		return 0;
	} else {
		return metric.extractor(dataElement.own);
	}
}

MattFuncMetrics.prototype.getFormattedValue = function(dataElement,metricName,inclusive)
{
	return maltMetrics[metricName].formalter(this.getValue(dataElement,metricName,inclusive));
}

MattFuncMetrics.prototype.getValueRatio = function(dataElement,metricName,inclusive)
{
	return (100 *this.getValue(dataElement,metricName,inclusive)) / this.ref[metricName];
}

MattFuncMetrics.prototype.getRef = function(data,metricName)
{
// 	if (this.refs == undefined)
// 		this.buildRefs(data);
// 	console.log(metricName + " " + this.refs[metricName]);
// 	return this.refs[metricName];
	return this.computeRef(data,metricName);
}

MattFuncMetrics.prototype.buildRefs = function(data)
{
	this.refs = {};
	for (var i in maltMetrics)
		this.refs[i] = this.computeRef(data,maltMetrics[i]);
}

MattFuncMetrics.prototype.computeRef = function(data,metricName)
{
	var metric = maltMetrics[metricName];
	var res = 0;
	switch(metric.ref)
	{
		case 'sum':
			for (var i in data)
				res += this.getValue(data[i],metricName);
			break;
		case 'max':
			for (var i in data)
			{
				var tmp = this.getValue(data[i],metricName);
				if (tmp > res)
					res = tmp;
			}
			break;
		default:
			console.log("Invalid value for ref mode for metric "+metricName+" : "+metric.ref);
			res = 1;
	}

	return res;
}

function MattSelector()
{
	this.funcMetrics = new MattFuncMetrics();
	this.metric = 'alloc.count';
	this.inclusive = true;
	this.limit = 10;
	this.ratio = false;
	this.query='';
	this.order = maltMetrics[this.metric].defaultOrder;
	this.functions = [];
	this.currentPage = 1;
	this.perPage = 10;
	this.totalElements = 0;
}

MattSelector.prototype.setData = function(data)
{
	this.functions = data;
	this.onInternalChange();
}

MattSelector.prototype.onInternalChange = function()
{
	var cnt = 0;
	for(var i in this.functions)
	{
		if (this.filter(this.functions[i]))
			cnt++;
	}
	this.totalElements = cnt;
	console.log(cnt);
	this.onChange();
}

MattSelector.prototype.getValue = function(x)
{
	return this.funcMetrics.getValue(x,this.metric,this.inclusive);
}

MattSelector.prototype.computeRef = function() 
{
	return this.funcMetrics.getRef(this.functions,this.metric);
}

MattSelector.prototype.getValueRatio = function(x) 
{
	return (100 *this.getValue(x)) / this.computeRef();
}

MattSelector.prototype.getFormattedValue = function(x) 
{
	if (this.ratio)
	{
		return this.getValueRatio(x).toFixed(1)+"%";
	} else {
		return this.funcMetrics.getFormattedValue(x,this.metric,this.inclusive);
	}
}

MattSelector.prototype.filter = function(x) 
{
	return (this.getValue(x) > 0 && (this.query == '' || x.function.indexOf(this.query) > -1));
}

MattSelector.prototype.isReversedOrder = function () 
{
	return (this.order == 'desc');
}

MattSelector.prototype.accepted = function(x)
{
	return (this.getValue(x) > 0 && (this.query == '' || x.function.indexOf(this.query) > -1));
}

MattSelector.prototype.toogleOrder = function()
{
	this.order = (this.order == 'asc')?'desc':'asc';
	this.onInternalChange();
}

MattSelector.prototype.toogleRatio = function()
{
	this.ratio =  !this.ratio;
	this.onInternalChange();
}

MattSelector.prototype.toogleInclusive = function()
{
	this.inclusive = !this.inclusive;
	this.onInternalChange();
}

MattSelector.prototype.getCurMetricName = function()
{
	return maltMetrics[this.metric].name;
}

MattSelector.prototype.selectMetric = function(metric)
{
	this.metric = metric.key;
	this.order = maltMetrics[this.metric].defaultOrder;
	this.onInternalChange();
}

MattSelector.prototype.onChange = function()
{
	
}