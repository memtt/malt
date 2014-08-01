var selectedLibs = "all";
var varData = {};
var varMaxThreadCount = 1;

/////////////////////////////// PIES /////////////////////////////////////////////
function cutForPie(data,curPercent,maxNb,ignorePercent)
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

function getDataForBinaryPies(data)
{
	var res = [];
	
	//generate
	for (var bin in data)
	{
		var cnt = 0;
		for (var v in data[bin])
		{
			var cur = data[bin][v];
			if (cur.tls)
				cnt += cur.size * varMaxThreadCount;
			else
				cnt += cur.size;
		}
		res.push({name:bin.split('/').pop(),value:cnt});
	}
	
	return res;
}

function getDataForVarPies(data)
{
	var res = [];
	
	//generate
	for (var bin in data)
	{
		var cnt = 0;
		for (var v in data[bin])
		{
			var cur = data[bin][v];
			var size = cur.size;
			if (cur.tls)
				size *= varMaxThreadCount;
			res.push({binary:bin,name:cur.name,value:size,file:cur.file,line:cur.line});
		}
	}
	
	return res;
}

//matt-vars-pie-bin
function buildDonutGraph(domId,data)
{
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
			console.log(y);
			console.log(e);
			var d = data[e.pointIndex];
			var pos = "";
			if (d.file != undefined && d.file != '')
				pos = "<br/>" + d.file + ":" + d.line;
			return "<div style='text-align:center'><h3>"+d.name+"</h3>"+mattHelper.humanReadableValue(d.value,'B',false)+pos+'</div>';
		});

		d3.select("#"+domId)
			.datum(data)
			.transition().duration(350)
			.call(chart);

	return chart;
	});
}

////////////////////////////// SUMMARY /////////////////////////////////
function getSummary(data)
{
	var tls = 0;
	var gbl = 0;
	for (var bin in data)
	{
		for (var v in data[bin])
		{
			if (data[bin][v].tls)
				tls += data[bin][v].size;
			else
				gbl += data[bin][v].size;
		}
	}
	
	return {global:gbl,tls:tls * varMaxThreadCount};
}

/////////////////////////// BAR CHART /////////////////////////////////
function getDataForBinaryGraphs(data)
{
	var res = [];
	
	//generate
	for (var bin in data)
	{
		var cntTls = 0;
		var cntGbl = 0;
		for (var v in data[bin])
		{
			var cur = data[bin][v];
			if (cur.tls)
				cntTls += cur.size * varMaxThreadCount;
			else
				cntGbl += cur.size;
		}
		res.push({name:bin.split('/').pop(),tls:cntTls,gbl:cntGbl});
	}
	
	return formatDatasForBarChar(res);
}

function getDataForVarGraphs(data)
{
	var res = [];
	
	//generate
	for (var bin in data)
	{
		var fname = bin.split('/').pop();
		if (selectedLibs == "all" || selectedLibs[fname] == true)
		{
			var cntTls = 0;
			var cntGbl = 0;
			for (var v in data[bin])
			{
					var cur = data[bin][v];
					if (cur.tls)
						res.push({name:cur.name,tls:cur.size * varMaxThreadCount,gbl:0});
					else
						res.push({name:cur.name,tls:0,gbl:cur.size});
			}
			//res.push({name:bin,tls:cntTls,gbl:cntGbl});
		}
	}
	
	return formatDatasForBarChar(cutDataForBarChart(res));
}

function cutDataForBarChart(data)
{
	//no filter
	if (data.length < 16)
		return data;

	//sort
	data = data.sort(function(a,b){return (b.tls - a.tls)+(b.gbl - a.gbl)});

	//count up to 95%
	var tot = 0;
	var totTLS = 0;
	var totGLB = 0;
	for (var i in data)
	{
		tot += data[i].tls + data[i].gbl;
		totTLS += data[i].tls;
		totGLB += data[i].gbl;
	}
	
	//cut at 95%
	var res = [];
	var cur = 0;
	var curTLS = 0;
	var curGLB = 0;
	var cnt = 0;
	for (var i in data)
	{
		if ((100*cur)/tot < 95 && cnt < 300)
		{
			cnt++;
			cur += data[i].tls + data[i].gbl;
			res.push(data[i]);
		} else {
			curTLS += data[i].tls;
			curGLB += data[i].gbl;
			break;
		}
	}
	
	if (curTLS > 0 || curGLB > 0)
		res.push({name:'OTHERS',tls:curTLS,gbl:curGLB});
	
	return res;
}

function formatDatasForBarChar(data)
{
	var gbl = [];
	var tls = [];
	
	//sorte
	dataSorted = data.sort(function(a,b){return (b.tls - a.tls)+(b.gbl - a.gbl)});
	
	//split
	for (var i in dataSorted)
	{
		gbl.push({name:dataSorted[i].name,value:dataSorted[i].gbl});
		tls.push({name:dataSorted[i].name,value:dataSorted[i].tls});
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

function buildMultiBarChart(domId,data,onClick)
{
	nv.addGraph(function() {
		$("#"+domId).height( 150+data[0].values.length * 16 );
		
		var chart = nv.models.multiBarHorizontalChart()
			.x(function(d) { return d.name })
			.y(function(d) { return d.value })
			.margin({top: 30, right: 20, bottom: 50, left: 175})
			.showValues(true)           //Show bar value next to each bar.
			.tooltips(true)             //Show tooltips on hover.
			.transitionDuration(350)
			.showControls(true)
			.stacked(true);        //Allow user to switch between "Grouped" and "Stacked" mode.

		chart.yAxis
			.tickFormat(d3.format(',.2f'));
			
		if (onClick != undefined)
			chart.multibar.dispatch.on("elementClick", function(e) {onClick(e,chart);});

		d3.select('#'+domId)
			.datum(data)
			.call(chart);
		
		nv.utils.windowResize(chart.update);

		return chart;
	});
}

function onClickForBinChart(e,chart) {
			console.log("Want to move to source of : "+e.series.values[e.pointIndex].name + " or via 'point' : "+JSON.stringify(e.point));
			console.log(e);
			selectedLibs = {};
			selectedLibs[e.point.name] = true;
			
			
			var d = getDataForVarGraphs(varData);
			$('#matt-vars-multibar-var').height( 150+d[0].values.length * 16 );

			nv.utils.windowResize(chart.update);
			
			d3.select('#matt-vars-multibar-var')
				.datum(d)
// 				.transition().duration(500)
				.call(chart);
				
			//d3.select('#'+domId+' .nv-groups g rect').style('color','');
			//d3.select(e.e.target).select('rect').style('color','gray');
};