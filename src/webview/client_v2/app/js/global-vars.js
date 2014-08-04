function MattPageGlobalVars()
{
	var defaultData = {};
	var cur = this;
	var svgId = 0;
	
	//declare module to manage matt home page
	var mattModule = angular.module('matt.page.globalVars',[]);
	
	//main controler of the page
	var pageCtrl = mattCtrl.controller('matt.page.globalVars.ctrl',['$scope','$http',function($scope,$http) {
		//fetch summaryData
		$http.get('/global-variables.json').success(function(data) {
			//keep link to data
			$scope.globalVars = data;
			
			//extract some infos for view
			var summary = cur.getSummary(data);
			$scope.globalVariablesMem = mattHelper.humanReadable(summary.global,1,'B',false);
			$scope.tlsVariablesMem = mattHelper.humanReadable(summary.tls,1,'B',false);
			$scope.activeThreads = data.maxThreadCount;
			
			//gen formatted data
			$scope.dataForBinaryPie = cur.cutForPie(cur.getDataForBinaryPies(data),95,8,0.1);
			$scope.dataForVarPie = cur.cutForPie(cur.getDataForVarPies(data),95,16,0.1);
			$scope.dataForBinaryBar = cur.getDataForBinaryGraphs(data);
			$scope.dataForVarBar = cur.getDataForVarGraphs(data);
			
			$scope.onVarChartValueClick = function(e,chart,data,rscope)
			{
				console.log('filter on : '+e.point.name);
				$scope.dataForVarBar = cur.getDataForVarGraphs($scope.globalVars,function(x) {return x == e.point.name;});
				$scope.$apply();
			}
		});
	}]);

	//directive to put pie graphs
	pageCtrl.directive('mattgblvarchart',function() {
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
								cur.buildDonutGraph('#mattgblvarchart' + $scope.elementID + ' svg',$scope.data);
								break;
							case 'bar':
								if ($scope.chart == undefined)
									cur.buildMultiBarChart($scope,'#mattgblvarchart' + $scope.elementID + ' svg',$scope.data,function(e,chart,data){$scope.onvalueclick(e,chart,data,$scope);});
								else
									cur.updateMultiBarChart($scope,'#mattgblvarchart' + $scope.elementID + ' svg',$scope.data);
								break;
							default:
								console.log("Invalid kinf of matt gbl var chart : "+$scope.kind);
						}
					}
				});
			},
			controller: ['$scope', '$element', '$attrs', function($scope, $element, $attrs){
				if ($scope.elementID == undefined)
				{
					//setup element ID
					$scope.elementID = svgId++;
					angular.element($element).attr('id', 'mattgblvarchart'+$scope.elementID );
				}
				
				if ($scope.height != undefined)
					d3.select('#mattgblvarchart' + $scope.elementID + ' svg').attr('height',$scope.height);
			}]
		}
	});
}

////////////////////////////// SUMMARY /////////////////////////////////
//function to build summary
MattPageGlobalVars.prototype.getSummary = function(data)
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
	
	return {global:gbl,tls:tls * data.maxThreadCount};
}

/////////////////////////////// PIES /////////////////////////////////////////////
MattPageGlobalVars.prototype.cutForPie = function(data,curPercent,maxNb,ignorePercent)
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
			if ((100*(tot-cum))/tot < ignorePercent)
				finalRes.push({name:'others',value:(tot-cum)});
			break;
		} else {
			finalRes.push(sortedRes[i]);
		}
	}
	
	return finalRes;
}

MattPageGlobalVars.prototype.getDataForBinaryPies = function(data)
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
				cnt += cur.size * data.maxThreadCount;
			else
				cnt += cur.size;
		}
		res.push({name:bin.split('/').pop(),value:cnt});
	}
	
	return res;
}

MattPageGlobalVars.prototype.getDataForVarPies = function(data)
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
				size *= data.maxThreadCount;
			res.push({binary:bin,name:cur.name,value:size,file:cur.file,line:cur.line});
		}
	}
	
	return res;
}

//matt-vars-pie-bin
MattPageGlobalVars.prototype.buildDonutGraph = function(d3Selection,data)
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
		.tooltipContent(function(key,y,e,graph) {
			var d = data[e.pointIndex];
			var pos = "";
			if (d.file != undefined && d.file != '')
				pos = "<br/>" + d.file + ":" + d.line;
			return "<div style='text-align:center'><h3>"+d.name+"</h3>"+mattHelper.humanReadable(d.value,1,'B',false)+pos+'</div>';
		});

		d3.select(d3Selection)
			.datum(data)
			.transition().duration(350)
			.call(chart);

	return chart;
	});
}

/////////////////////////// BAR CHART /////////////////////////////////
MattPageGlobalVars.prototype.getDataForBinaryGraphs = function(data)
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
				cntTls += cur.size * data.maxThreadCount;
			else
				cntGbl += cur.size;
		}
		res.push({name:bin.split('/').pop(),tls:cntTls,gbl:cntGbl});
	}
	
	return this.formatDatasForBarChar(res);
}

MattPageGlobalVars.prototype.getDataForVarGraphs = function(data,filter)
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
						res.push({name:cur.name,tls:cur.size * data.maxThreadCount,gbl:0,file:cur.file,line:cur.line});
					else
						res.push({name:cur.name,tls:0,gbl:cur.size,file:cur.file,line:cur.line});
			}
			//res.push({name:bin,tls:cntTls,gbl:cntGbl});
		}
	}
	
	return this.formatDatasForBarChar(this.cutDataForBarChart(res));
}

MattPageGlobalVars.prototype.cutDataForBarChart = function(vars)
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

MattPageGlobalVars.prototype.formatDatasForBarChar = function(vars)
{
	var gbl = [];
	var tls = [];
	
	//sorte
	varsSorted = vars.sort(function(a,b){return (b.tls - a.tls)+(b.gbl - a.gbl)});
	
	//split
	for (var i in varsSorted)
	{
		var cur = varsSorted[i];
		gbl.push({name:varsSorted[i].name,value:varsSorted[i].gbl,file:cur.file,line:cur.line});
		tls.push({name:varsSorted[i].name,value:varsSorted[i].tls,file:cur.file,line:cur.line});
	}
	
	//format final output
	return [
		{
			key:'Global variables',
			color:'blue',
			values:gbl
		} , {
			key:'TLS variables',
			color:'red',
			values:tls
		}
	];
}

MattPageGlobalVars.prototype.updateMultiBarChart = function($scope,d3Selection,data)
{
	var chart = $scope;
	d3.select(d3Selection).attr('height', 150+data[0].values.length * 16 );

	nv.utils.windowResize(chart.update);

	d3.select(d3Selection)
			.datum(data)
			.call(chart);
}

MattPageGlobalVars.prototype.buildMultiBarChart = function($scope,d3Selection,data,onClick)
{
	nv.addGraph(function() {
		d3.select(d3Selection).attr('height', 150+data[0].values.length * 16 );

		var chart = nv.models.multiBarHorizontalChart()
			.x(function(d) { return d.name })
			.y(function(d) { return d.value })
			.margin({top: 30, right: 20, bottom: 50, left: 175})
			.showValues(true)           //Show bar value next to each bar.
			.tooltips(true)             //Show tooltips on hover.
			.transitionDuration(350)
			.showControls(true)
			.stacked(true)        //Allow user to switch between "Grouped" and "Stacked" mode.
			.tooltipContent(function(serieName,name,value,e,graph) {
				var d = data[e.seriesIndex].values[e.pointIndex];
				var pos = "";
				if (d.file != undefined && d.file != '')
					pos = "<br/>" + d.file + ":" + d.line;
				return "<div style='text-align:center'><h3>"+d.name+"</h3>"+mattHelper.humanReadable(d.value,1,'B',false)+pos+'</div>';
			});

		$scope = chart;
		chart.yAxis
			.tickFormat(d3.format(',.2f'));
			
		if (onClick != undefined)
			chart.multibar.dispatch.on("elementClick", function(e) {onClick(e,chart);});

		d3.select(d3Selection)
			.datum(data)
			.call(chart);
		
		nv.utils.windowResize(chart.update);

		return chart;
	});
}

//init and export
var mattPageGlobalVars = new MattPageGlobalVars();
