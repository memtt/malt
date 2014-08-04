//declare metrics
var mattMetrics = {
	'alloc.sum': {
		name: 'Allocated mem.',
		extractor: function(x) {return x.alloc.sum;},
		formatter: function(x) {return mattHelper.humanReadable(x,1,'B',false);},
		defaultOrder: 'desc',
		ref: 'sum'
	},
	'alloc.count': {
		name: 'Allocated count',
		extractor: function(x) {return x.alloc.count;},
		formatter: function(x) {return mattHelper.humanReadable(x,1,'',false);},
		defaultOrder: 'desc',
		ref: 'sum'
	},
	'alloc.min': {
		name: 'Min. alloc size',
		extractor: function(x) {return x.alloc.min;},
		formatter: function(x) {return mattHelper.humanReadable(x,1,'B',false);},
		defaultOrder: 'asc',
		ref: 'max'
	},
	'alloc.mean': {
		name: 'Mean alloc size',
		extractor: function(x) {return x.alloc.count == 0 ? 0 : x.alloc.sum / x.alloc.count;},
		formatter: function(x) {return mattHelper.humanReadable(x,1,'B',false);},
		defaultOrder: 'asc',
		ref: 'max'
	},
	'alloc.max': {
		name: 'Max. alloc size',
		extractor: function(x) {return x.alloc.max;},
		formatter: function(x) {return mattHelper.humanReadable(x,1,'B',false);},
		defaultOrder: 'desc',
		ref: 'max'
	},
	'free.sum': {
		name: 'Freed mem.',
		extractor: function(x) {return x.free.sum;},
		formatter: function(x) {return mattHelper.humanReadable(x,1,'B',false);},
		defaultOrder: 'desc',
		ref: 'sum'
	},
	'free.count': {
		name: 'Free count',
		extractor: function(x) {return x.free.count;},
		formatter: function(x) {return mattHelper.humanReadable(x,1,'',false);},
		defaultOrder: 'desc',
		ref: 'sum'
	},
	'memops': {
		name: 'Memory ops.',
		extractor: function(x) {return x.alloc.count + x.free.count;},
		formatter: function(x) {return mattHelper.humanReadable(x,1,'',false);},
		defaultOrder: 'desc',
		ref: 'sum'
	},
	'peakmem.local': {
		name: 'Local peak',
		extractor: function(x) {return x.maxAliveReq;},
		formatter: function(x) {return mattHelper.humanReadable(x,1,'B',false);},
		defaultOrder: 'desc',
		ref: 'max'
	},
	'peakmem.global': {
		name: 'Global peak',
		extractor: function(x) {return x.globalPeak;},
		formatter: function(x) {return mattHelper.humanReadable(x,1,'B',false);},
		defaultOrder: 'desc',
		ref: 'sum'
	},
	'leaks': {
		name: 'Leaks',
		extractor: function(x) {return x.aliveReq;},
		formatter: function(x) {return mattHelper.humanReadable(x,1,'B',false);},
		defaultOrder: 'desc',
		ref: 'sum'
	},
	'lifetime.max': {
		name: 'Max lifetime',
		extractor: function(x) {return x.lifetime.max;},
		formatter: function(x) {return mattHelper.humanReadable(x,1,'',false);},
		defaultOrder: 'desc',
		ref: 'max'
	},
	'lifetime.min': {
		name: 'Max lifetime',
		extractor: function(x) {return x.lifetime.min;},
		formatter: function(x) {return mattHelper.humanReadable(x,1,'',false);},
		defaultOrder: 'asc',
		ref: 'max'
	},
	'recycling.ratio': {
		name: 'Recycling ratio',
		extractor: function(x) {return x.alloc.count == 0 ? 1 : x.alloc.sum / x.alloc.count;},
		formatter: function(x) {return mattHelper.humanReadable(x,1,'',false);},
		defaultOrder: 'desc',
		ref: 'max'
	},
};

function MattFuncMetrics()
{
}

MattFuncMetrics.prototype.getMetricNames = function()
{
	var res = [];
	for (var i in mattMetrics)
		res.push(mattMetrics[i].name);
	return res;
}

MattFuncMetrics.prototype.getValue = function(dataElement,metricName,inclusive)
{
	var metric = mattMetrics[metricName];

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
	return mattMetrics[metricName].formatter(this.getValue(dataElement,metricName,inclusive));
}

MattFuncMetrics.prototype.getValueRatio = function(dataElement,metricName,inclusive)
{
	return (100 *this.getValue(dataElement,metricName,inclusive)) / this.ref[metricName];
}

MattFuncMetrics.prototype.getRef = function(data,metricName)
{
	return this.computeRef(data,metricName);
}

MattFuncMetrics.prototype.buildRefs = function(data)
{
	this.refs = {};
	for (var i in mattMetrics)
		this.refs[i] = this.computeRef(data,mattMetrics[i]);
}

MattFuncMetrics.prototype.computeRef = function(data,metricName)
{
	var metric = mattMetrics[metricName];
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
