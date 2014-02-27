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

function mattConvertData(data)
{
	var res = new Array();
	res.push( mattConvertDataInternalD3JS(data.virtualMem) );
	res.push( mattConvertDataInternalD3JS(data.requestedMem) );
	res.push( mattConvertDataInternalD3JS(data.physicalMem) );
	var labels = new Array();
	labels.push("Virtual memory");
	labels.push("Requested memory");
	labels.push("Physical memory");
	return {data:res,labels:labels};
}

function mattConvertCnt(data)
{
	var res = new Array();
	res.push( mattConvertDataInternalD3JS(data.segments) );
	var labels = new Array();
	labels.push("Memory segments");
	return {data:res,labels:labels};
}

function mattUpdatePlot(id,title,data,unit)
{			
	var plot = $.jqplot(id, data.data, {
		title: title,
		stackSeries: false,
		seriesDefaults: {
			showMarker: false,
			fill: true,
			fillAndStroke: true
		},
		legend: {
			show: true,
			placement: 'outsideGrid',
			renderer: $.jqplot.EnhancedLegendRenderer,
			labels: data.labels,
			location: 'ne',
			rowSpacing: '0px'
		},
		axesDefaults: {
			labelRenderer: $.jqplot.CanvasAxisLabelRenderer
		},
		cursor:{
			show: true,
			zoom:true,
			showTooltip:true
		} ,
		axes: {
			xaxis: {
				pad: 0,
// 							ticks: ticks,
				label: 'Time',
				tickOptions: {
					showGridline: false,
					formatter: function(format,value) {return mattHelper.humanReadableValue(value,'');}
				}
			},
			yaxis: {
				min: 0,
				label: 'Memory',
				tickOptions: {
				showGridline: false,
				formatter: function(format,value) {return mattHelper.humanReadableValue(value,unit);}
				}
			}
		},
		grid: {
			drawGridLines: true, 
			gridLineColor: '#cccccc',
			background: '#464D56',
			borderColor: '#999999',
			drawBorder: false,
			shadow: false,
			renderer: $.jqplot.CanvasGridRenderer,
			// background: 'rgba(0,0,0,0)'  works to make transparent.
// 						background: 'white'
		}
	});
	
	var d = new $.jsDate();
	$('div.jqplot-datestamp').html('Generated on '+d.strftime('%v'));
	
// 				$("div.chart-container").resizable({delay:20});
	$('div.chart-container').bind('resize', function(event, ui) {
		plot.replot();
	});
	
		$('#matt-plot').bind('jqplotDataHighlight', function(ev, seriesIndex, pointIndex, data) {
		var idx = 21 - seriesIndex
		$('tr.jqplot-table-legend').removeClass('legend-row-highlighted'); 
		$('tr.jqplot-table-legend').children('.jqplot-table-legend-label').removeClass('legend-text-highlighted');
		$('tr.jqplot-table-legend').eq(idx).addClass('legend-row-highlighted');
		$('tr.jqplot-table-legend').eq(idx).children('.jqplot-table-legend-label').addClass('legend-text-highlighted');
	});

	$('#matt-plot').bind('jqplotDataUnhighlight', function(ev, seriesIndex, pointIndex, data) {
		$('tr.jqplot-table-legend').removeClass('legend-row-highlighted'); 
		$('tr.jqplot-table-legend').children('.jqplot-table-legend-label').removeClass('legend-text-highlighted');
	});
}


function mattNVDGraph(divId,data)
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
			.axisLabel('Time (cycles)')
			.tickFormat(function(value){return mattHelper.humanReadableValue(value,'');});
		
		chart.yAxis //Chart y-axis settings
			.axisLabel('Memory')
			.tickFormat(function(value){return mattHelper.humanReadableValue(value,'');});
		
		/* Done setting the chart up? Time to render it!*/
		var myData = sinAndCos(data); //You need data...
		
		d3.select('#'+divId + " svg") //Select the <svg> element you want to render the chart in.
			.datum(myData) //Populate the <svg> element with chart data...
			.call(chart); //Finally, render the chart!

		d3.select('#'+divId + " svg g g rect").attr('style','').classed('matt-graph-background',true);
		
		//Update the chart when window resizes.
		nv.utils.windowResize(function() { chart.update() });
		return chart;
	});
}

/**************************************
* Simple test data generator
*/
function sinAndCos(data) {
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

function basename(path) {
   return path.split('/').reverse()[0];
}

function mattProcMapPie(divId,data,selector)
{
	var tmp = new Array();
	var sum = 0;
	for (var i in data)
	{
		var value = selector(data[i]);
		sum += value;
		tmp.push({label:basename(i),value:value});
	}
	
	tmp.sort(function(a,b){return b.value - a.value});
	var cut = tmp;
	if (tmp.length > 15)
	{
		cut = tmp.splice(0,15);
		var partialSum = 0;
		for (var i in cut)
			partialSum += cut[i].value;
		cut.push({label:'others',value:sum-partialSum});
	}
	
	nv.addGraph(function() {
		var chart = nv.models.pieChart()
			.x(function(d) { return d.label })
			.y(function(d) { return d.value })
			.showLabels(true)
			.labelThreshold(.05)
			.labelType("percent");
		
		d3.select("#"+divId+" svg")
			.datum(cut)
			.transition().duration(350)
			.call(chart);
		
		return chart;
	});
}


