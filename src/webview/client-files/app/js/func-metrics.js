if(typeof maltHelper === 'undefined') {
	maltHelper = new (require('./helper.js'))();
}

//declare metrics
function MaltFuncMetrics()
{
}

MaltFuncMetrics.prototype.maltMetrics = {
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
	'realloc.count': {
		name: 'Realloc count',
		extractor: function(x) {return x.reallocCount;},
		formalter: function(x) {return maltHelper.humanReadable(x,1,'',false);},
		defaultOrder: 'desc',
		ref: 'sum'
	},
	'realloc.sum': {
		name: 'Realloc sum',
		extractor: function(x) {return x.reallocSumDelta;},
		formalter: function(x) {return maltHelper.humanReadable(x,1,'',false);},
		defaultOrder: 'desc',
		ref: 'max'
	}
};

MaltFuncMetrics.prototype.getMetricList = function()
{
	var ret = [];
	for (var i in this.maltMetrics)
	{
		ret.push({name:this.maltMetrics[i].name,key:i});
	}
	return ret;
}

MaltFuncMetrics.prototype.getMetricNames = function()
{
	var res = [];
	for (var i in this.maltMetrics)
		res.push(this.maltMetrics[i].name);
	return res;
}

MaltFuncMetrics.prototype.getValue = function(dataElement,metricName,inclusive)
{
	var metric = this.maltMetrics[metricName];

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

MaltFuncMetrics.prototype.getFormattedValue = function(dataElement,metricName,inclusive)
{
	return this.maltMetrics[metricName].formalter(this.getValue(dataElement,metricName,inclusive));
}

MaltFuncMetrics.prototype.getValueRatio = function(dataElement,metricName,inclusive)
{
	return (100 *this.getValue(dataElement,metricName,inclusive)) / this.ref[metricName];
}

MaltFuncMetrics.prototype.getRef = function(data,metricName)
{
// 	if (this.refs == undefined)
// 		this.buildRefs(data);
// 	console.log(metricName + " " + this.refs[metricName]);
// 	return this.refs[metricName];
	return this.computeRef(data,metricName);
}

MaltFuncMetrics.prototype.buildRefs = function(data)
{
	this.refs = {};
	for (var i in this.maltMetrics)
		this.refs[i] = this.computeRef(data,this.maltMetrics[i]);
}

MaltFuncMetrics.prototype.computeRef = function(data,metricName)
{
	var metric = this.maltMetrics[metricName];
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

// If this is ran on server-side, we export the class as a module
// Otherwise, we create a globally shared object for this class
if(typeof exports !== 'undefined') {
	module.exports = MaltFuncMetrics;
}