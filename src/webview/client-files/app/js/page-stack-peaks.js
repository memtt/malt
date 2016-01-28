function MaltPageStackPeaks()
{
	//declare module to manage malt home page
	var maltModule = angular.module('malt.page.stackPeaks',[]);
	var svgId = 0;
	var cur = this;
	
	//main controler of the page
	var pageCtrl = maltCtrl.controller('malt.page.stackPeaks.ctrl',['$scope','$http',function($scope,$http) {
		//fetch summaryData
		maltDataSource.loadStackData($http,function(data) {
			$scope.stackMem = data;
			$scope.selectedThreadId = 0;
			
			//search largest
			var largest = 0;
			for (var i in data.stacks)
				if (data.stacks[i].size > largest)
					largest = data.stacks[i].size;
			
			//export
			$scope.largestStack = maltHelper.humanReadable(largest,1,"B",false);
			$scope.threads = data.stacks.length;
			
			var selectedChart = cur.plotSelectedStackSize('malt-selected-stack-size',data,-1);
			
			var selectedChartDetails = cur.plotStackMemOfFuncs('malt-selected-stack-details',0);
			
			cur.plotStackPeak('malt-stack-peaks',data, function(svg,chart,e) {
				console.log(e);
				$scope.selectedThreadId = e.pointIndex;
				$scope.$apply();
				selectedChart.update(data,e.pointIndex);
				selectedChartDetails.update(e.pointIndex);
			});
		});
	}]);
}

function reformatDataForD3(data,threadId) {
	var res =  [];
	
	for (var j in data.stacks)
	{
		if (threadId == -1 || threadId == j)
		{
			var d = [];
			for (var i in data.stacks[j].timeprofiler.max)
				d.push({
					x: data.stacks[j].timeprofiler.timestamp[i],
					y: data.stacks[j].timeprofiler.max[i]
				});
			
			res.push({
						values: d, //values - represents the array of {x,y} data points
						key: "Thread "+j, //key - the name of the series.
						//color: colors[i], //color - optional: choose your own line color.
						area:true
					});
		}
	}

	return res;
}

MaltPageStackPeaks.prototype.plotSelectedStackSize = function(domId,data,threadId)
{
	var chart;
	nv.addGraph(function() {
		chart = nv.models.lineChart()
			.margin({top: 30, right: 20, bottom: 50, left: 200})
			.useInteractiveGuideline(true) //We want nice looking tooltips and a guideline!
// 			.transitionDuration(500) //how fast do you want the lines to transition?
			.showLegend(true) //Show the legend, allowing users to turn on/off line series.
			.showYAxis(true) //Show the y-axis
			.showXAxis(true) //Show the x-axis
		;
		
// 		chart = nv.models.lineWithFocusChart()
// 			.transitionDuration(500) //how fast do you want the lines to transition?
// 		;
		
		chart.xAxis //Chart x-axis settings
			.axisLabel('Time (secondes)')
			.tickFormat(function(value){return maltHelper.humanReadable(value/(+data.ticksPerSecond),1,'s');});
		
		chart.yAxis //Chart y-axis settings
			.axisLabel("Stack size")
			.tickFormat(function(value){return maltHelper.humanReadable(value,1,'B');});
		
		/* Done setting the chart up? Time to render it!*/
		var formattedData = reformatDataForD3(data,threadId); //You need data...
		
		d3.select('#'+domId + " svg") //Select the <svg> element you want to render the chart in.
			.datum(formattedData) //Populate the <svg> element with chart data...
			.call(chart); //Finally, render the chart!

		//d3.select('#'+divId + " svg g g rect").attr('style','').classed('malt-graph-background',true);
		
		//Update the chart when window resizes.
		nv.utils.windowResize(function() { chart.update() });
		return chart;
	});
	
	return {
		update: function(data,threadId) {
			console.log("Update data for threadId " +threadId);
			d3.select('#'+domId+' svg')
				.datum(reformatDataForD3(data,threadId))
				.transition().duration(500)
				.call(chart);
		}
	};
}

MaltPageStackPeaks.prototype.plotStackMemOfFuncs = function(domId,threadId,onElementClick)
{
	var cur = this;
	var chart;
	nv.addGraph(function() {
		chart = nv.models.multiBarHorizontalChart()
			.x(function(d,i) { return d.info.function })
			.y(function(d) { return d.mem })
			.margin({top: 30, right: 20, bottom: 50, left: 200})
			.showValues(true)           //Show bar value next to each bar.
// 			.tooltips(true)             //Show tooltips on hover.
// 			.transitionDuration(350)
			.showControls(false)
			.stacked(true)  ;      //Allow user to switch between "Grouped" and "Stacked" mode.
// 			.tooltipContent(function(serieName,name,value,e,graph) {
// 				var d = e.series.values[e.pointIndex];
// 				var pos = "";
// 				var tls = "";
// 				if (d.file != undefined && d.file != '')
// 					pos = "<br/>" + d.file + ":" + d.line;
// 				if (e.series.key == "TLS variables")
// 					tls = " [ "+e.series.tlsInstances+" * "+maltHelper.humanReadable(d.value/e.series.tlsInstances,1,'B',false) +" ] ";
// 				var ratio = " ( "+(100*d.value/e.series.total).toFixed(2)+"% ) ";
// 				//console.log(data);
// 				//console.log(e);
// 				return "<div style='text-align:center'><h3>"+d.name+"</h3>"+maltHelper.humanReadable(d.value,1,'B',false)+tls+ratio+pos+'</div>';
// 			});

		chart.yAxis
			.axisLabel('Stack memory')
			.tickFormat(function(d) {return maltHelper.humanReadable(d,1,'B',false);});
			
// 		chart.xAxis
// 			.axisLabel('Function');
			
		if (onElementClick != undefined)
		{
			chart.multibar.dispatch.on("elementClick", function(e) {
				onElementClick(d3.select("#"+domId+" svg"),chart,e);
			});
		}

		d3.json('/stack.json?id='+threadId, function(data) {
			//prepeare data
			var formattedData = [
				{
					key:'Frame size',
					color:'blue',
					values:data.details
				}
			];
			
			d3.select("#"+domId+" svg").attr('height', 150+data.details.length * 16 );
			
			console.log(formattedData);
			d3.select("#"+domId+" svg")
				.datum(formattedData)
				.call(chart);

			nv.utils.windowResize(chart.update);
		});

		return chart;
	});
	
	return {
		update: function(threadId) {
			d3.json('/stack.json?id='+threadId, function(data) {
				//prepeare data
				var formattedData = [
					{
						key:'Frame size',
						color:'blue',
						values:data.details
					}
				];
				
				d3.select("#"+domId+" svg").attr('height', 150+data.details.length * 16 );
				
				console.log(formattedData);
				console.log("Update data for threadId " +threadId);
				d3.select('#'+domId+' svg')
					.datum(formattedData)
					.transition().duration(500)
					.call(chart);

				nv.utils.windowResize(chart.update);
			});
		}
	};
}

MaltPageStackPeaks.prototype.plotStackPeak = function(domId,data,onElementClick)
{
	var cur = this;
	nv.addGraph(function() {
		d3.select("#"+domId+" svg").attr('height', 150+data.stacks.length * 16 );
		
		//prepeare data
		var formattedData = [
			{
				key:'Stack',
				color:'blue',
				values:data.stacks
			}
		];

		var chart = nv.models.multiBarHorizontalChart()
			.x(function(d,i) { return i })
			.y(function(d) { return d.size })
			.margin({top: 30, right: 20, bottom: 50, left: 40})
			.showValues(true)           //Show bar value next to each bar.
// 			.tooltips(true)             //Show tooltips on hover.
// 			.transitionDuration(350)
			.showControls(false)
			.stacked(true)  ;      //Allow user to switch between "Grouped" and "Stacked" mode.
// 			.tooltipContent(function(serieName,name,value,e,graph) {
// 				var d = e.series.values[e.pointIndex];
// 				var pos = "";
// 				var tls = "";
// 				if (d.file != undefined && d.file != '')
// 					pos = "<br/>" + d.file + ":" + d.line;
// 				if (e.series.key == "TLS variables")
// 					tls = " [ "+e.series.tlsInstances+" * "+maltHelper.humanReadable(d.value/e.series.tlsInstances,1,'B',false) +" ] ";
// 				var ratio = " ( "+(100*d.value/e.series.total).toFixed(2)+"% ) ";
// 				//console.log(data);
// 				//console.log(e);
// 				return "<div style='text-align:center'><h3>"+d.name+"</h3>"+maltHelper.humanReadable(d.value,1,'B',false)+tls+ratio+pos+'</div>';
// 			});

// 		$scope.chart = chart;
		chart.yAxis
			.axisLabel('Maximum stack size')
			.tickFormat(function(d) {return maltHelper.humanReadable(d,1,'B',false);});
			
		chart.xAxis
			.axisLabel('Thread ID');
			
		if (onElementClick != undefined)
		{
			chart.multibar.dispatch.on("elementClick", function(e) {
				onElementClick(d3.select("#"+domId+" svg"),chart,e);
			});
		}
			
// 		if (onClick != undefined)
// 			chart.multibar.dispatch.on("elementClick", function(e) {onClick(e,chart);});
		
		//labels
// 		d3.select(d3Selection).append("text")
// 			.attr("class", "x label")
// 			.attr("text-anchor", "end")
// 			.attr("x", width)
// 			.attr("y", height - 6)
// 			.text("Virtual size (B)");
		
// 		svg.append("text")
// 			.attr("class", "y label")
// 			.attr("text-anchor", "end")
// 			.attr("y", 6)
// 			.attr("dy", ".75em")
// 			.attr("transform", "rotate(-90)")
// 			.text("ylabel");

		d3.select("#"+domId+" svg")
			.datum(formattedData)
			.call(chart);
		
		nv.utils.windowResize(chart.update);

		return chart;
	});
}

var maltPageStackPeaks = new MaltPageStackPeaks();
