function MaltPageGlobalVars()
{
	//an example of data format
	var defaultData = {
		maxThreadCount:1,
		vars: {
			"/usr/libc.so": [ /* Binary containing the variables */
				{
					name:'a', /* Name of the variable */
					size:32,  /* Size of the variable in bytes. */
					tls:false,/* Variable is TLS or standard global. */
					//optional :
					file:"/test.c",
					line:10
				}
			]
		}
	};
	var cur = this;
	var svgId = 0;
	
	//declare module to manage malt home page
	var maltModule = angular.module('malt.page.globalVars',[]);
	
	//main controler of the page
	var pageCtrl = maltCtrl.controller('malt.page.globalVars.ctrl',['$scope','$http',function($scope,$http) {
		//fetch summaryData
		maltDataSource.loadGlobalVars($http,function(data) {
			//keep link to data
			$scope.globalVars = data;
			
			//extract some infos for view
			var summary = cur.getSummary(data);
			$scope.globalVariablesMem = maltHelper.humanReadable(summary.global,1,'B',false);
			$scope.tlsVariablesMem = maltHelper.humanReadable(summary.tls,1,'B',false);
			$scope.activeThreads = data.maxThreadCount;
			
			//gen formatted data
			$scope.dataForBinaryPie = cur.cutForPie(cur.getDataForBinaryPies(data),95,8,0.1);
			$scope.dataForVarPie = cur.cutForPie(cur.getDataForVarPies(data),95,16,0.1);
			$scope.dataForBinaryBar = cur.getDataForBinaryGraphs(data);
			$scope.dataForVarBar = cur.getDataForVarGraphs(data);
			
			$scope.onVarChartValueClick = function(e,chart,data,rscope)
			{
				//console.log('filter on : '+e.point.name);
				$scope.dataForVarBar = cur.getDataForVarGraphs($scope.globalVars,function(x) {return x == e.point.name;});
				$scope.$apply();
			}
		});
	}]);

	//directive to put pie graphs
	pageCtrl.directive('maltgblvarchart',function() {
		return {
			restrict: 'EA',
			template: '<div><svg/></div>',
			replace: true,
			scope: {
				data:'=',
				height:'@',
				kind:'@',
				onvalueclick:'='
			},
			link: function ($scope) {
				$scope.$watch('data', function(data){
					if ($scope.data != undefined && $scope.elementID != undefined)
					{
						switch($scope.kind)
						{
							case 'pie':
								cur.buildDonutGraph('#maltgblvarchart' + $scope.elementID + ' svg',$scope.data);
								break;
							case 'bar':
								if ($scope.chart == undefined)
									cur.buildMultiBarChart($scope,'#maltgblvarchart' + $scope.elementID + ' svg',$scope.data,function(e,chart,data){$scope.onvalueclick(e,chart,data,$scope);});
								else
									cur.updateMultiBarChart($scope,'#maltgblvarchart' + $scope.elementID + ' svg',$scope.data);
								break;
							default:
								console.log("Invalid kinf of malt gbl var chart : "+$scope.kind);
						}
					}
				});
			},
			controller: ['$scope', '$element', '$attrs', function($scope, $element, $attrs){
				if ($scope.elementID == undefined)
				{
					//setup element ID
					$scope.elementID = svgId++;
					angular.element($element).attr('id', 'maltgblvarchart'+$scope.elementID );
				}
				
				if ($scope.height != undefined)
					d3.select('#maltgblvarchart' + $scope.elementID + ' svg').attr('height',$scope.height);
			}]
		}
	});
}

MaltPageGlobalVars.prototype.getTLSInstances = function(data)
{
	//1 instance of TLS per thread plus the one to store defualt values
	return data.maxThreadCount + 1;
}

////////////////////////////// SUMMARY /////////////////////////////////
//function to build summary
MaltPageGlobalVars.prototype.getSummary = function(data)
{
	var tls = 0;
	var gbl = 0;
	var vars = data.vars;
	for (var bin in vars)
	{
		for (var v in vars[bin])
		{
			if (vars[bin][v].tls)
				tls += vars[bin][v].size;
			else
				gbl += vars[bin][v].size;
		}
	}
	
	return {global:gbl,tls:tls * this.getTLSInstances(data)};
}

/////////////////////////////// PIES /////////////////////////////////////////////
MaltPageGlobalVars.prototype.cutForPie = function(data,curPercent,maxNb,ignorePercent)
{	
	//compute tot
	var tot = 0;
	for (var i in data)
		tot += data[i].value;

	//sort
	sortedRes = data.sort(function(a,b) {return b.value - a.value;});
	
	//search first 97%
	var cum = 0;
	var finalRes = [];
	for (var i in sortedRes)
	{
		cum += sortedRes[i].value;
		if (((100 * cum) / tot > curPercent && sortedRes.length - i >= 3) || i >= maxNb)
		{
// 			if ((100*(tot-cum))/tot < ignorePercent)
				finalRes.push({name:'others',value:(tot-cum)});
			break;
		} else {
			finalRes.push(sortedRes[i]);
		}
	}
	
	return finalRes;
}

MaltPageGlobalVars.prototype.getDataForBinaryPies = function(data)
{
	var res = [];
	var vars = data.vars;
	
	//generate
	for (var bin in vars)
	{
		var cnt = 0;
		for (var v in vars[bin])
		{
			var cur = vars[bin][v];
			if (cur.tls)
				cnt += cur.size * this.getTLSInstances(data);
			else
				cnt += cur.size;
		}
		res.push({name:bin.split('/').pop(),value:cnt});
	}
	
	return res;
}

MaltPageGlobalVars.prototype.getDataForVarPies = function(data)
{
	var res = [];
	var vars = data.vars;
	
	//generate
	for (var bin in vars)
	{
		var cnt = 0;
		for (var v in vars[bin])
		{
			var cur = vars[bin][v];
			var size = cur.size;
			if (cur.tls)
				size *= this.getTLSInstances(data);
			res.push({binary:bin,name:cur.name,value:size,file:cur.file,line:cur.line});
		}
	}
	
	return res;
}

//malt-vars-pie-bin
MaltPageGlobalVars.prototype.buildDonutGraph = function(d3Selection,data)
{
	if (data == undefined)
		return;

	//Donut chart example
	nv.addGraph(function() {
	var chart = nv.models.pieChart()
		.x(function(d) { return d.name })
		.y(function(d) { return d.value })
		.showLabels(true)     //Display pie labels
		.labelThreshold(.05)  //Configure the minimum slice size for labels to show up
		.labelType("percent") //Configure what type of data to show in the label. Can be "key", "value" or "percent"
		.donut(true)          //Turn on Donut mode. Makes pie chart look tasty!
		.donutRatio(0.35)     //Configure how big you want the donut hole size to be.
		/*.tooltip(function(key,y,e,graph) {
			var d = data[e.pointIndex];
			var pos = "";
			if (d.file != undefined && d.file != '')
				pos = "<br/>" + d.file + ":" + d.line;
			return "<div style='text-align:center'><h3>"+d.name+"</h3>"+maltHelper.humanReadable(d.value,1,'B',false)+pos+'</div>';
		})*/;
		
// 		chart.tooltip.valueFormatter(function (d) { return maltHelper.humanReadable(d,1,'B',false); });
		chart.tooltip.contentGenerator(function(obj) {
			var d = obj.data;
			var pos = "";
			if (d.file != undefined && d.file != '')
				pos = "<br/>" + d.file + ":" + d.line;
			return "<div style='text-align:center'><h3>"+d.name+"</h3>"+maltHelper.humanReadable(d.value,1,'B',false)+pos+'</div>';
		});

		d3.select(d3Selection)
			.datum(data)
			.transition().duration(350)
			.call(chart);

	return chart;
	});
}

/////////////////////////// BAR CHART /////////////////////////////////
MaltPageGlobalVars.prototype.getDataForBinaryGraphs = function(data)
{
	var res = [];
	var vars = data.vars;
	
	//generate
	for (var bin in vars)
	{
		var cntTls = 0;
		var cntGbl = 0;
		for (var v in vars[bin])
		{
			var cur = vars[bin][v];
			if (cur.tls)
				cntTls += cur.size * this.getTLSInstances(data);
			else
				cntGbl += cur.size;
		}
		res.push({name:bin.split('/').pop(),tls:cntTls,gbl:cntGbl});
	}
	
	return this.formatDatasForBarChar(data,res);
}

MaltPageGlobalVars.prototype.getDataForVarGraphs = function(data,filter)
{
	var res = [];
	var vars = data.vars;
	
	//generate
	for (var bin in vars)
	{
		var fname = bin.split('/').pop();
		if (filter==undefined || filter(fname))
		{
			var cntTls = 0;
			var cntGbl = 0;
			for (var v in vars[bin])
			{
					var cur = vars[bin][v];
					if (cur.tls)
						res.push({name:cur.name,tls:cur.size * this.getTLSInstances(data),gbl:0,file:cur.file,line:cur.line});
					else
						res.push({name:cur.name,tls:0,gbl:cur.size,file:cur.file,line:cur.line});
			}
			//res.push({name:bin,tls:cntTls,gbl:cntGbl});
		}
	}
	
	return this.formatDatasForBarChar(data,this.cutDataForBarChart(res));
}

MaltPageGlobalVars.prototype.cutDataForBarChart = function(vars)
{
	//no filter
	if (vars.length < 16)
		return vars;

	//sort
	vars = vars.sort(function(a,b){return (b.tls - a.tls)+(b.gbl - a.gbl)});

	//count up to 95%
	var tot = 0;
	var totTLS = 0;
	var totGLB = 0;
	for (var i in vars)
	{
		tot += vars[i].tls + vars[i].gbl;
		totTLS += vars[i].tls;
		totGLB += vars[i].gbl;
	}
	
	//cut at 95%
	var res = [];
	var cur = 0;
	var curTLS = 0;
	var curGLB = 0;
	var cnt = 0;
	for (var i in vars)
	{
		if ((100*cur)/tot < 95 && cnt < 300)
		{
			cnt++;
			cur += vars[i].tls + vars[i].gbl;
			res.push(vars[i]);
		} else {
			curTLS += vars[i].tls;
			curGLB += vars[i].gbl;
			break;
		}
	}
	
	if (curTLS > 0 || curGLB > 0)
		res.push({name:'OTHERS',tls:curTLS,gbl:curGLB});
	
	return res;
}

MaltPageGlobalVars.prototype.formatDatasForBarChar = function(data,vars)
{
	var gbl = [];
	var tls = [];
	var sum = 0;
	
	//sorte
	varsSorted = vars.sort(function(a,b){return (b.tls - a.tls)+(b.gbl - a.gbl)});
	
	//split
	for (var i in varsSorted)
	{
		var cur = varsSorted[i];
		sum += varsSorted[i].gbl + varsSorted[i].tls;
		gbl.push({name:varsSorted[i].name,value:varsSorted[i].gbl,file:cur.file,line:cur.line});
		tls.push({name:varsSorted[i].name,value:varsSorted[i].tls,file:cur.file,line:cur.line});
	}
	
	//format final output
	return [
		{
			key:'Global variables',
			color:'blue',
			values:gbl,
			total: sum
		} , {
			key:'TLS variables',
			color:'red',
			values:tls,
			tlsInstances: this.getTLSInstances(data),
			total: sum
		}
	];
}

MaltPageGlobalVars.prototype.updateMultiBarChart = function($scope,d3Selection,data)
{
	var chart = $scope.chart;
	
	d3.select(d3Selection).attr('height', 150+data[0].values.length * 16 );

	nv.utils.windowResize(chart.update);

	d3.select(d3Selection)
			.datum(data)
			.call(chart);
}

MaltPageGlobalVars.prototype.buildMultiBarChart = function($scope,d3Selection,data,onClick)
{
	var cur = this;
	nv.addGraph(function() {
		d3.select(d3Selection).attr('height', 150+data[0].values.length * 16 );

		var chart = nv.models.multiBarHorizontalChart()
			.x(function(d) { return d.name })
			.y(function(d) { return d.value })
			.margin({top: 30, right: 20, bottom: 50, left: 175})
			.showValues(true)           //Show bar value next to each bar.
// 			.tooltips(true)             //Show tooltips on hover.
// 			.transitionDuration(350)
			.showControls(true)
			.stacked(true)        //Allow user to switch between "Grouped" and "Stacked" mode.
			/*.tooltip(function(serieName,name,value,e,graph) {
				var d = e.series.values[e.pointIndex];
				var pos = "";
				var tls = "";
				if (d.file != undefined && d.file != '')
					pos = "<br/>" + d.file + ":" + d.line;
				if (e.series.key == "TLS variables")
					tls = " [ "+e.series.tlsInstances+" * "+maltHelper.humanReadable(d.value/e.series.tlsInstances,1,'B',false) +" ] ";
				var ratio = " ( "+(100*d.value/e.series.total).toFixed(2)+"% ) ";
				//console.log(data);
				//console.log(e);
				return "<div style='text-align:center'><h3>"+d.name+"</h3>"+maltHelper.humanReadable(d.value,1,'B',false)+tls+ratio+pos+'</div>';
			})*/;
			
		var tlsInstances = cur.getTLSInstances(data);
		chart.tooltip.contentGenerator(function (obj) {
				console.log(obj);
				var d = obj.data;
				var pos = "";
				var tls = "";
				if (d.file != undefined && d.file != '')
					pos = "<br/>" + d.file + ":" + d.line;
				if (d.key == "TLS variables")
					tls = " [ "+tlsInstances+" * "+maltHelper.humanReadable(d.value/tlsInstances,1,'B',false) +" ] ";
// 				var ratio = " ( "+(100*d.value/e.series.total).toFixed(2)+"% ) ";
				var ratio = "";
				//console.log(data);
				//console.log(e);
				return "<div style='text-align:center'><h3>"+d.name+"</h3>"+maltHelper.humanReadable(d.value,1,'B',false)+tls+ratio+pos+'</div>';
		});

		$scope.chart = chart;
		chart.yAxis
			.tickFormat(function(d) {return maltHelper.humanReadable(d,1,'B',false);});
			
		if (onClick != undefined)
			chart.multibar.dispatch.on("elementClick", function(e) {onClick(e,chart);});
		
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
			.datum(data)
			.call(chart);
		
		nv.utils.windowResize(chart.update);

		return chart;
	});
}

//init and export
var maltPageGlobalVars = new MaltPageGlobalVars();
