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
	
	function mattNVDGraph2(divId,data,ylabel,stacked)
	{
		nv.addGraph(function() {
			var chart = nv.models.stackedAreaChart()
				.transitionDuration(350)
// 				.reduceXTicks(true)   //If 'false', every single x-axis tick label will be rendered.
// 				.rotateLabels(0)      //Angle to rotate x-axis labels.
// 				.showControls(true)   //Allow user to switch between 'Grouped' and 'Stacked' mode.
// 				.groupSpacing(0.1)    //Distance between each group of bars.
				.useInteractiveGuideline(true)    //Tooltips which show all data points. Very nice!
// 				.showControls(true)
// 				.stacked(true)        //Allow user to switch between "Grouped" and "Stacked" mode.
				;

			chart.xAxis //Chart x-axis settings
				.axisLabel('Time (secondes)')
				.tickFormat(function(value){return mattHelper.humanReadableTimes(value/(+data.ticksPerSecond),1,'',false);});
			
			chart.yAxis //Chart y-axis settings
				.axisLabel(ylabel)
				.tickFormat(function(value){return mattHelper.humanReadable(value,1,'',false);});

			var myData = reformatDataForD3(data); //You need data...
				
			d3.select('#'+divId + " svg")
				.datum(myData)
				.call(chart);

			nv.utils.windowResize(chart.update);

			return chart;
		});
	}

	function mattNVDGraph(divId,data,ylabel,stacked)
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
			
// 			chart.interactiveLayer.dispatch.on("elementMousemove", function(e) {
// 				alert('ok');
// 				console.log(e);
// 			});
			
			chart.xAxis //Chart x-axis settings
				.axisLabel('Time (secondes)')
				.tickFormat(function(value){return mattHelper.humanReadableTimes(value/(+data.ticksPerSecond),1,'',false);});
			
			chart.yAxis //Chart y-axis settings
				.axisLabel(ylabel)
				.tickFormat(function(value){return mattHelper.humanReadable(value,1,'',false);});
				
// 			chart.lines.dispatch.on('elementClick', function(e) {
// 				alert("You've clicked on " + e.series.key + " - " + e.point.x);
// 			});
			
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
	
	function mattConvertDataInternalD3JS2(data,id)
	{
		var res = new Array();
		res.push({x:0,y:0});
		console.log("scale data : "+data.perPoints);
		for (var i = 0 ; i < data.values.length ; i++)
		{
			res.push({x:(i+1)*data.perPoints,y:data.values[i][id]});
		}
		return res;
	}

	function mattConvertDataInternalRateD3JS(data,ticksPerSecond)
	{
		var res = new Array();
		console.log(data.scale);
		//alert(data.startTime + " -> "+data.endTime+" -> "+data.steps);
		for (var i in data.values)
		{
			if (data.startTime + data.scale*i <= data.endTime)
				res.push({x:(data.scale*i),y:((data.values[i])/(data.scale/ticksPerSecond))});
		}
		return res;
	}
	
	function mattConvertDataInternalRateD3JS2(data,id,ticksPerSecond)
	{
		var res = new Array();
		var scale = data.perPoints;
		console.log("scake rate : "+scale+" => "+(600*scale/ticksPerSecond));
		//alert(data.startTime + " -> "+data.endTime+" -> "+data.steps);
		res.push({x:0,y:0});
		console.log("=> "+data.perPoint);
		for (var i in data.values)
		{
			//if (data.startTime + data.scale*i <= data.endTime)
// 			console.log(data.perPoints + " => " +data.values.length + " => " + i + " => "+(i+1)*data.perPoints/ticksPerSecond + " => "+data.values[i][id]);
			res.push({x:(scale*(i+1)),y:((data.values[i][id])/(data.perPoints/ticksPerSecond))});
		}
		console.log(res);
		return res;
	}


	function mattConvertData(data)
	{
		var res = new Array();
		res.push( mattConvertDataInternalD3JS2(data.memoryTimeline,1) );
		res.push( mattConvertDataInternalD3JS2(data.memoryTimeline,2) );
		res.push( mattConvertDataInternalD3JS2(data.memoryTimeline,0) );
		
		var labels = new Array();
		labels.push("Virtual memory");
		labels.push("Physical memory");
		labels.push("Requested memory");
		
		return {data:res,labels:labels,ticksPerSecond:data.ticksPerSecond};
	}
	
	function mattConvertData2(data)
	{
		var res = new Array();
		res.push( mattConvertDataInternalD3JS2(data.memoryTimeline,1) );
		res.push( mattConvertDataInternalD3JS2(data.memoryTimeline,2) );
		res.push( mattConvertDataInternalD3JS2(data.memoryTimeline,0) );
		
		var labels = new Array();
		labels.push("Virtual memory");
		labels.push("Physical memory");
		labels.push("Requested memory");
		
		return {data:res,labels:labels,ticksPerSecond:data.ticksPerSecond};
	}
	
	function mattConvertSysData(data)
	{
		var res = new Array();
		res.push( mattConvertDataInternalD3JS2(data.systemTimeline,0) );
		res.push( mattConvertDataInternalD3JS2(data.systemTimeline,1) );
		//res.push( mattConvertDataInternalD3JS2(data.systemTimeline,2) );
		
		var labels = new Array();
		labels.push("System free memory");
		labels.push("Swap memory");
		//labels.push("Cached memory");
		
		return {data:res,labels:labels,ticksPerSecond:data.ticksPerSecond};
	}

	function mattConvertCnt(data)
	{
		var res = new Array();
		res.push( mattConvertDataInternalD3JS2(data.memoryTimeline,4) );
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
		labels.push("Malloc");
		labels.push("Free");
		return {data:res,labels:labels,ticksPerSecond:ticksPerSecond};
	}
	
	function mattConvertSizeRate2(data)
	{
		var ticksPerSecond = +data.ticksPerSecond;
		//ticksPerSecond = 1;
		
		var res = new Array();
		res.push( mattConvertDataInternalRateD3JS2(data.memoryBandwidth,1,ticksPerSecond) );
		res.push( mattConvertDataInternalRateD3JS2(data.memoryBandwidth,3,ticksPerSecond) );

		var labels = new Array();
		labels.push("Malloc");
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
		labels.push("Malloc");
		labels.push("Free");
		return {data:res,labels:labels,ticksPerSecond:ticksPerSecond};
	}
	
	function mattConvertCountRate2(data)
	{
		var ticksPerSecond = data.ticksPerSecond;
		//ticksPerSecond = 1;
		
		var res = new Array();
		res.push( mattConvertDataInternalRateD3JS2(data.memoryBandwidth,0,ticksPerSecond) );
		res.push( mattConvertDataInternalRateD3JS2(data.memoryBandwidth,2,ticksPerSecond) );

		var labels = new Array();
		labels.push("Malloc");
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
		mattDataSource.loadTimedData($http,function(data) {
			$scope.reallocMap = data;

			mattNVDGraph("matt-mem-timeline",mattConvertData(data),'B','Memory');
			mattNVDGraph("matt-alive-chunks-timeline",mattConvertCnt(data),'Alive chunks','Allocations');
			mattNVDGraph2("matt-alloc-rate-size-timeline",mattConvertSizeRate2(data),'Allocation rate B/s','Memory rate (B/s)');
			mattNVDGraph2("matt-alloc-rate-count-timeline",mattConvertCountRate2(data),'Allocation rate op/s','Memory rate (B/s)');
			mattNVDGraph("matt-sys-free-mem-timeline",mattConvertSysData(data),"B","Memory");
		});
	}]);
}

var mattPageTimeline = new MattPageTimeline();
