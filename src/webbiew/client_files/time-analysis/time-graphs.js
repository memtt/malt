function mattConvertDataInternal(data)
{
	var res = new Array();
	for (var i = 0 ; i < data.timestamp.length ; i++)
	{
		res[i] = [data.timestamp[i],data.max[i]];
	}
	return res;
}

function mattConvertData(data)
{
	var res = new Array();
	res.push( mattConvertDataInternal(data.virtualMem) );
	res.push( mattConvertDataInternal(data.physicalMem) );
	res.push( mattConvertDataInternal(data.requestedMem) );
	res.push( mattConvertDataInternal(data.internalMem) );
	var labels = new Array();
	labels.push("Virtuel memory");
	labels.push("Physical memory");
	labels.push("Requested memory");
	labels.push("Internal memory");
	return {data:res,labels:labels};
}

function mattConvertCnt(data)
{
	var res = new Array();
	res.push( mattConvertDataInternal(data.segments) );
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