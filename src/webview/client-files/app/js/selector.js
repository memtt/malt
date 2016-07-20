function MaltSelector()
{
	this.funcMetrics = new MaltFuncMetrics();
	this.metric = 'alloc.count';
	this.inclusive = true;
	this.limit = 10;
	this.ratio = false;
	this.query='';
	this.order = this.funcMetrics.maltMetrics[this.metric].defaultOrder;
	this.functions = [];
	this.currentPage = 1;
	this.perPage = 10;
	this.totalElements = 0;
}

MaltSelector.prototype.setData = function(data)
{
	this.functions = data;
	this.onInternalChange();
}

MaltSelector.prototype.onInternalChange = function()
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

MaltSelector.prototype.getValue = function(x)
{
	return this.funcMetrics.getValue(x,this.metric,this.inclusive);
}

MaltSelector.prototype.computeRef = function() 
{
	return this.funcMetrics.getRef(this.functions,this.metric);
}

MaltSelector.prototype.getValueRatio = function(x) 
{
	return (100 *this.getValue(x)) / this.computeRef();
}

MaltSelector.prototype.getFormattedValue = function(x) 
{
	if (this.ratio)
	{
		return this.getValueRatio(x).toFixed(1)+"%";
	} else {
		return this.funcMetrics.getFormattedValue(x,this.metric,this.inclusive);
	}
}

MaltSelector.prototype.filter = function(x) 
{
	return (this.getValue(x) > 0 && (this.query == '' || x.function.indexOf(this.query) > -1));
}

MaltSelector.prototype.isReversedOrder = function () 
{
	return (this.order == 'desc');
}

MaltSelector.prototype.accepted = function(x)
{
	return (this.getValue(x) > 0 && (this.query == '' || x.function.indexOf(this.query) > -1));
}

MaltSelector.prototype.toogleOrder = function()
{
	this.order = (this.order == 'asc')?'desc':'asc';
	this.onInternalChange();
}

MaltSelector.prototype.toogleRatio = function()
{
	this.ratio =  !this.ratio;
	this.onInternalChange();
}

MaltSelector.prototype.toogleInclusive = function()
{
	this.inclusive = !this.inclusive;
	this.onInternalChange();
}

MaltSelector.prototype.getCurMetricName = function()
{
	return this.funcMetrics.maltMetrics[this.metric].name;
}

MaltSelector.prototype.selectMetric = function(metric)
{
	this.metric = metric.key;
	this.order = this.funcMetrics.maltMetrics[this.metric].defaultOrder;
	this.onInternalChange();
}

MaltSelector.prototype.onChange = function()
{
	
}