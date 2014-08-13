function MattPageStackPeaks()
{
	//declare module to manage matt home page
	var mattModule = angular.module('matt.page.stackPeaks',[]);
	var svgId = 0;
	var cur = this;
	
	//main controler of the page
	var pageCtrl = mattCtrl.controller('matt.page.stackPeaks.ctrl',['$scope','$http',function($scope,$http) {
		//fetch summaryData
		$http.get('/stacks-mem.json').success(function(data) {
			$scope.stackMem = data;
			
			//search largest
			var largest = 0;
			for (var i in data.stacks)
				if (data.stacks[i].size > largest)
					largest = data.stacks[i].size;
			
			//export
			$scope.largestStack = mattHelper.humanReadable(largest,1,"B",false);
			
			console.log(data);
			cur.plotStackPeak('matt-stack-peaks',data);
			cur.plotSelectedStackSize('matt-selected-stack-size',data,0);
		});
	}]);
}

function reformatDataForD3(data,threadId) {
	var d = [];
	console.log(data);
	for (var i in data.stacks[threadId].timeprofiler.max)
		d.push([
			data.stacks[threadId].timeprofiler.timestamp[i],
			data.stacks[threadId].timeprofiler.max[i]]);
	
	res = [{
				values: d, //values - represents the array of {x,y} data points
				key: threadId, //key - the name of the series.
				//color: colors[i], //color - optional: choose your own line color.
				area:true
			}];

	console.log(res);
	return res;
}

MattPageStackPeaks.prototype.plotSelectedStackSize = function(domId,data,threadId)
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
			.tickFormat(function(value){return mattHelper.humanReadable(value/(+data.ticksPerSecond),1,'');});
		
		chart.yAxis //Chart y-axis settings
			.axisLabel("label")
			.tickFormat(function(value){return mattHelper.humanReadable(value,1,'B');});
		
		/* Done setting the chart up? Time to render it!*/
		var formattedData = reformatDataForD3(data,threadId); //You need data...
		
		d3.select('#'+domId + " svg") //Select the <svg> element you want to render the chart in.
			.datum(formattedData) //Populate the <svg> element with chart data...
			.call(chart); //Finally, render the chart!

		//d3.select('#'+divId + " svg g g rect").attr('style','').classed('matt-graph-background',true);
		
		//Update the chart when window resizes.
		nv.utils.windowResize(function() { chart.update() });
		return chart;
	});
}

MattPageStackPeaks.prototype.plotStackPeak = function(domId,data)
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
			.tooltips(true)             //Show tooltips on hover.
			.transitionDuration(350)
			.showControls(false)
			.stacked(true)  ;      //Allow user to switch between "Grouped" and "Stacked" mode.
// 			.tooltipContent(function(serieName,name,value,e,graph) {
// 				var d = e.series.values[e.pointIndex];
// 				var pos = "";
// 				var tls = "";
// 				if (d.file != undefined && d.file != '')
// 					pos = "<br/>" + d.file + ":" + d.line;
// 				if (e.series.key == "TLS variables")
// 					tls = " [ "+e.series.tlsInstances+" * "+mattHelper.humanReadable(d.value/e.series.tlsInstances,1,'B',false) +" ] ";
// 				var ratio = " ( "+(100*d.value/e.series.total).toFixed(2)+"% ) ";
// 				//console.log(data);
// 				//console.log(e);
// 				return "<div style='text-align:center'><h3>"+d.name+"</h3>"+mattHelper.humanReadable(d.value,1,'B',false)+tls+ratio+pos+'</div>';
// 			});

// 		$scope.chart = chart;
		chart.yAxis
			.tickFormat(function(d) {return mattHelper.humanReadable(d,1,'B',false);});
			
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

var mattPageStackPeaks = new MattPageStackPeaks();