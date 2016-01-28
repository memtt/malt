function MaltPagePerThread()
{
	var cur = this;

	//declare module to manage malt home page
	var maltModule = angular.module('malt.page.perthread',[]);
	
	//main controler of the page
	var homeCtrl = maltCtrl.controller('malt.page.perthread.ctrl',['$scope','$http',function($scope,$http) {
		//setup data
// 		$scope.summaryViewEntries = summaryViewEntries;
		$scope.displaySummaryLevel = 1;
		$scope.displaySummaryHelp = false;
// 		$scope.summaryData = defaultData;
		
		//fetch summaryData
		maltDataSource.loadGlobalSummary($http,function(data) {
			$scope.summaryData = data;
			cur.buildPerThreadChart(data,"#malt-per-thread-funcs-count svg","count");
			cur.buildPerThreadChart(data,"#malt-per-thread-funcs-time svg","time");
			cur.buildPerThreadChart(data,"#malt-per-thread-funcs-sum svg","sum");
		});
	}]);
}

MaltPagePerThread.prototype.buildPerThreadChart = function(data,d3Selection,metric)
{
	//format data
	var formattedData = [];
	var allocFuncs = ["malloc","free","calloc","realloc","posix_memalign","aligned_alloc","memalign","valloc","pvalloc"];
	for (var i in allocFuncs)
	{
		var values = [];
		for (var j in data.threadStats)
		{
			values.push({
				name:"Thread "+j,
				value: data.threadStats[j][allocFuncs[i]][metric]
			});
		}
		
		formattedData.push({
			key: allocFuncs[i],
			values: values
		});
	}
	
	var cur = this;
	nv.addGraph(function() {
		d3.select(d3Selection).attr('height', 150+data.threadStats.length * 16 );

		var chart = nv.models.multiBarHorizontalChart()
			.x(function(d) { return d.name })
			.y(function(d) { return d.value })
			.margin({top: 30, right: 20, bottom: 50, left: 175})
			.showValues(true)           //Show bar value next to each bar.
// 			.tooltips(true)             //Show tooltips on hover.
// 			.transitionDuration(350)
			.showControls(true)
			.stacked(true)        //Allow user to switch between "Grouped" and "Stacked" mode.
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
// 			})
			;

// 		$scope.chart = chart;
		chart.yAxis
			.tickFormat(function(d) {return maltHelper.humanReadable(d,1,'',false);});
			
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

		d3.select(d3Selection)
			.datum(formattedData)
			.transition().duration(350)
			.call(chart);
		
		nv.utils.windowResize(chart.update);
	});
}

//init and export
var maltPagePerThread = new MaltPagePerThread();
