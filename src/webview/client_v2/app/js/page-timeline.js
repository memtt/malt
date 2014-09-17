function MattPageTimeline()
{
	function reformatDataForD3(data) {
		var res = new Array();
		var colors = ['#ff7f0e','#2ca02c','#7777ff'];
		
		for (var i in data.labels)
		{
			res.push(
				{
					values: data.data[i], //values - represents the array of {x,y} data points
					key: data.labels[i], //key - the name of the series.
					color: colors[i], //color - optional: choose your own line color.
					area:true
				}
			);
		}

		return res;
	}

	function mattNVDGraph(divId,data,ylabel)
	{
		nv.addGraph(function() {
			var chart = nv.models.lineChart()
				.margin({left: 100}) //Adjust chart margins to give the x-axis some breathing room.
				.useInteractiveGuideline(true) //We want nice looking tooltips and a guideline!
				.transitionDuration(500) //how fast do you want the lines to transition?
				.showLegend(true) //Show the legend, allowing users to turn on/off line series.
				.showYAxis(true) //Show the y-axis
				.showXAxis(true) //Show the x-axis
			;
			
			chart.xAxis //Chart x-axis settings
				.axisLabel('Time (secondes)')
				.tickFormat(function(value){return mattHelper.humanReadable(value/(+data.ticksPerSecond),1,'',false);});
			
			chart.yAxis //Chart y-axis settings
				.axisLabel(ylabel)
				.tickFormat(function(value){return mattHelper.humanReadable(value,1,'',false);});
			
			/* Done setting the chart up? Time to render it!*/
			var myData = reformatDataForD3(data); //You need data...
			
			d3.select('#'+divId + " svg") //Select the <svg> element you want to render the chart in.
				.datum(myData) //Populate the <svg> element with chart data...
				.call(chart); //Finally, render the chart!

			//d3.select('#'+divId + " svg g g rect").attr('style','').classed('matt-graph-background',true);
			
			//Update the chart when window resizes.
			nv.utils.windowResize(function() { chart.update() });
			return chart;
		});
	}
	
	function mattConvertDataInternal(data)
	{
		var res = new Array();
		for (var i = 0 ; i < data.timestamp.length ; i++)
		{
			res[i] = [data.timestamp[i],data.max[i]];
		}
		return res;
	}

	function mattConvertDataInternalD3JS(data)
	{
		var res = new Array();
		res.push({x:(data.timestamp[0]-1),y:0});
		for (var i = 0 ; i < data.timestamp.length ; i++)
		{
			res.push({x:data.timestamp[i],y:data.max[i]});
		}
		return res;
	}

	function mattConvertDataInternalRateD3JS(data,ticksPerSecond)
	{
		var res = new Array();
		//alert(data.startTime + " -> "+data.endTime+" -> "+data.steps);
		for (var i in data.values)
		{
			if (data.startTime + data.scale*i <= data.endTime)
				res.push({x:(data.scale*i),y:((data.values[i])/(data.scale/ticksPerSecond))});
		}
		return res;
	}

	function mattConvertData(data)
	{
		var res = new Array();
		res.push( mattConvertDataInternalD3JS(data.virtualMem) );
		res.push( mattConvertDataInternalD3JS(data.physicalMem) );
		res.push( mattConvertDataInternalD3JS(data.requestedMem) );
		
		var labels = new Array();
		labels.push("Virtual memory");
		labels.push("Physical memory");
		labels.push("Requested memory");
		
		return {data:res,labels:labels,ticksPerSecond:data.ticksPerSecond};
	}

	function mattConvertCnt(data)
	{
		var res = new Array();
		res.push( mattConvertDataInternalD3JS(data.segments) );
		var labels = new Array();
		labels.push("Memory segments");
		return {data:res,labels:labels,ticksPerSecond:data.ticksPerSecond};
	}

	function mattConvertSizeRate(data)
	{
		var ticksPerSecond = data.ticksPerSecond;
		//ticksPerSecond = 1;
		
		var res = new Array();
		res.push( mattConvertDataInternalRateD3JS(data.allocBandwidth,ticksPerSecond) );
		res.push( mattConvertDataInternalRateD3JS(data.freeBandwidth,ticksPerSecond) );

		var labels = new Array();
		labels.push("Alloc");
		labels.push("Free");
		return {data:res,labels:labels,ticksPerSecond:ticksPerSecond};
	}
	
	function mattConvertCountRate(data)
	{
		var ticksPerSecond = data.ticksPerSecond;
		//ticksPerSecond = 1;
		
		var res = new Array();
		res.push( mattConvertDataInternalRateD3JS(data.allocCnt,ticksPerSecond) );
		res.push( mattConvertDataInternalRateD3JS(data.freeCnt,ticksPerSecond) );

		var labels = new Array();
		labels.push("Alloc");
		labels.push("Free");
		return {data:res,labels:labels,ticksPerSecond:ticksPerSecond};
	}
	
	///////////////////////////////////// MAIN ////////////////////////////////////
	//declare module to manage matt home page
	var mattModule = angular.module('matt.page.timeline',[]);
	var svgId = 0;
	var cur = this;
	
	//main controler of the page
	var pageCtrl = mattCtrl.controller('matt.page.timeline.ctrl',['$scope','$http',function($scope,$http) {
		//fetch summaryData
		$http.get('/timed.json').success(function(data) {
			$scope.reallocMap = data;

			mattNVDGraph("matt-mem-timeline",mattConvertData(data),'B','Memory');
			mattNVDGraph("matt-alive-chunks-timeline",mattConvertCnt(data),'Alive chunks','Allocations');
			mattNVDGraph("matt-alloc-rate-size-timeline",mattConvertSizeRate(data),'Allocation rate B/s','Memory rate (B/s)');
			mattNVDGraph("matt-alloc-rate-count-timeline",mattConvertCountRate(data),'Allocation rate op/s','Memory rate (B/s)');
		});
	}]);
}

var mattPageTimeline = new MattPageTimeline();
