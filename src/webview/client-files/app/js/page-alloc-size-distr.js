function MaltPageAllocSizeDistr()
{
	//declare module to manage malt home page
	var maltModule = angular.module('malt.page.allocSizeDistr',[]);
	var svgId = 0;
	var cur = this;
	
	//main controler of the page
	var pageCtrl = maltCtrl.controller('malt.page.allocSizeDistr.ctrl',['$scope','$http',function($scope,$http) {
		//fetch summaryData
		maltDataSource.getSizeDistr($http,function(data) {
			$scope.sizeMap = data;
			
			//search summary
			var mostUsed = -1;
			var sumSize = 0;
			var sumCount = 0;
			var largest = 0;
			for (var i in data)
			{
				sumSize += +i;
				sumCount += +data[i];
				if (mostUsed == -1 || +data[mostUsed] < +data[i])
					mostUsed = +i;
				if (largest == 0 || largest < +i)
					largest = +i;
			}
			
			//Export
			$scope.meanChunkSize = maltHelper.humanReadable(sumSize / sumCount,1,'B',false);
			$scope.mostUsedSize = maltHelper.humanReadable(mostUsed,1,'B',false);
			$scope.largestSize = maltHelper.humanReadable(largest,1,'B',false);
			
			//charts
			cur.setupMostUsedChart('malt-most-used-sizes',data);
			cur.plotLogHisto('malt-size-distr-hist',data);
		});
	}]);
}

MaltPageAllocSizeDistr.prototype.buildLog2Histo = function(data)
{
	var tmp = [];
	
	for (var i in data)
	{
		var id = Math.ceil(Math.log(+i) / Math.log(2));
		if (tmp[id] == undefined)
			tmp[id] = +data[i];
		else
			tmp[id] += +data[i];
	}
	
	var hist=[];
	for (var i in tmp)
	{
		hist.push({
			sizeLog:+i,
			count:tmp[i],
		});
	}
	
	return hist;
}

MaltPageAllocSizeDistr.prototype.plotLogHisto = function(domId,data)
{
	var hist = this.buildLog2Histo(data);
	
	var margin = {top: 20, right: 20, bottom: 30, left: 60},
		width = (parseInt(d3.select('#'+domId+' svg').style('width')) || 960) - margin.left - margin.right,
		height = 500 - margin.top - margin.bottom;

	var formatPercent = d3.format(".0");

	var x = d3.scale.linear()
		.range([0, width]);

	var y = d3.scale.linear()
		.range([height, 0]);

	var xAxis = d3.svg.axis()
		.scale(x)
		.orient("bottom")
		.tickFormat(function(d) { return maltHelper.humanReadable(d==0?0:Math.pow(2,d-1),1,'B',false); });

	var yAxis = d3.svg.axis()
		.scale(y)
		.orient("left")
		.tickFormat(function(d) { return maltHelper.humanReadable(d,1,'',false); });

	var yAxisLines = d3.svg.axis()
		.scale(y)
		.orient("left")
		.tickFormat("");

	var svg = d3.select("#"+domId+' svg')
		.attr("width", width + margin.left + margin.right)
		.attr("height", height + margin.top + margin.bottom)
	.append("g")
		.attr("transform", "translate(" + margin.left + "," + margin.top + ")");
		
	var tip = d3.tip()
		.attr('class', 'd3-tip')
		.offset([-10, 0])
		.html(function(d,i) {
			return "<strong>"+ maltHelper.humanReadable(d.sizeLog==0?0:Math.pow(2,d.sizeLog-1),1,'B',false) + " - " + maltHelper.humanReadable(Math.pow(2,d.sizeLog),1,'B',false) +":</strong> <span style='color:red'>" + maltHelper.humanReadable(d.count,1,'',false) + "</span>";
		})

	svg.call(tip);

	x.domain([0, 1+d3.max(hist, function(d) { return d.sizeLog; })]);
	y.domain([0, d3.max(hist, function(d) { return d.count; })]);
	
	svg.append("g")         
		.attr("class", "grid")
		.call(yAxisLines
			.tickSize(-width, 0, 0)
			.tickFormat("")
		);

	svg.append("g")
		.attr("class", "x axis")
		.attr("transform", "translate(0," + height + ")")
		.call(xAxis);

	svg.append("g")
		.attr("class", "y axis")
		.call(yAxis)
		.append("text")
		.attr("transform", "rotate(-90)")
		.attr("y", 6)
		.attr("dy", ".71em")
		.style("text-anchor", "end")
		.text("Frequency");

	svg.selectAll(".bar")
		.data(hist)
		.enter().append("rect")
		.attr("class", "bar")
		.attr("x", function(d) { return x(d.sizeLog); })
		.attr("width", x(1) )
		.attr("y", function(d) { return y(d.count); })
		.attr("height", function(d) { return height - y(d.count); })
		.on('mouseover', tip.show)
		.on('mouseout', tip.hide);
}

/*MaltPageAllocSizeDistr.prototype.setupHistogramChart = function(domId,data)
{
	var hist = this.buildLogHisto(data);
	
	//TODO remove unused at end
	
	//format data
	var formattedData = [{
		key: 'Alloc size',
		color: 'blue',
		values: hist
		//values: [{name:0,count:1},{name:2,count:2}]
	}];

	//plot with nvd3
	nv.addGraph(function() {
		var chart = nv.models.multiBarChart()
			.x(function(d) { return d.name })
			.y(function(d) { return d.count })
			.transitionDuration(350)
			.reduceXTicks(false)   //If 'false', every single x-axis tick label will be rendered.
			.rotateLabels(0)      //Angle to rotate x-axis labels.
			.showControls(false)   //Allow user to switch between 'Grouped' and 'Stacked' mode.
			.stacked(true)
			.groupSpacing(0.1)    //Distance between each group of bars.
			;

// 		chart.xAxis
// 			.tickFormat(d3.format(',f'));

		chart.yAxis
			.tickFormat(function(d) { return maltHelper.humanReadable(d,1,'',false);});

		d3.select('#'+domId+' svg')
			.datum(formattedData)
			.call(chart);

		nv.utils.windowResize(chart.update);

		return chart;
	});
}*/

MaltPageAllocSizeDistr.prototype.setupMostUsedChart = function(domId,data)
{
	var margin = {top: 20, right: 20, bottom: 30, left: 60},
		width = (parseInt(d3.select('#'+domId+' svg').style('width')) || 960) - margin.left - margin.right,
		height = 500 - margin.top - margin.bottom;

	var formatPercent = d3.format(".0");

	var x = d3.scale.ordinal()
		.rangeRoundBands([0, width], .31, 1);

	var y = d3.scale.linear()
		.range([height, 0]);

	var xAxis = d3.svg.axis()
		.scale(x)
		.orient("bottom")
		.tickFormat(function(d) { if (d == 0) { return 'others'; } else { return maltHelper.humanReadable(d,1,'',false);} });

	var yAxis = d3.svg.axis()
		.scale(y)
		.orient("left")
		.tickFormat(function(d) { return maltHelper.humanReadable(d,1,'',false); });
		
	var yAxisLines = d3.svg.axis()
		.scale(y)
		.orient("left")
		.tickFormat("");

	var svg = d3.select("#"+domId+' svg')
		.attr("width", width + margin.left + margin.right)
		.attr("height", height + margin.top + margin.bottom)
	.append("g")
		.attr("transform", "translate(" + margin.left + "," + margin.top + ")");
		
	var tip = d3.tip()
		.attr('class', 'd3-tip')
		.offset([-10, 0])
		.html(function(d) {
			return "<strong>Frequency:</strong> <span style='color:red'>" + maltHelper.humanReadable(d.frequency,1,'',false) + "</span>";
		})

	svg.call(tip);

	//setup data
	var formattedData = [];
	var cnt = 0;
	var max = 0;
	var tmp = [];
	var sum = 0;
	
	for(var i in data)
	{
		tmp.push({size:+i,frequency:+data[i]});
		sum+=+data[i];
		if (data[i] > max)
			max = data[i];
	}
	
	tmp = tmp.sort(function(a,b) {return b.frequency - a.frequency;})
	
	var others = 0;
	for(var i in tmp)
	{
		//alert(i + " -> " + data[i]);
		if (tmp[i].frequency > max/1000 && cnt < 50)
		{
			formattedData.push(tmp[i]);
			cnt ++;
		} else {
			others += +tmp[i].frequency;
		}
	}
	formattedData.push({size:0,frequency:+others});
	formattedData = formattedData.sort(function(a,b) {return b.frequency - a.frequency;})
		
	data = formattedData;
	//alert(JSON.stringify(data));

	x.domain(data.map(function(d) { return d.size; }));
	y.domain([0, d3.max(data, function(d) { return d.frequency; })]);

	svg.append("g")
		.attr("class", "x axis")
		.attr("transform", "translate(0," + height + ")")
		.call(xAxis);
		
	svg.append("g")         
		.attr("class", "grid")
		.call(yAxisLines
			.tickSize(-width, 0, 0)
			.tickFormat("")
		);

	svg.append("g")
		.attr("class", "y axis")
		.call(yAxis)
		.append("text")
		.attr("transform", "rotate(-90)")
		.attr("y", 6)
		.attr("dy", ".71em")
		.style("text-anchor", "end")
		.text("Frequency");

	svg.selectAll(".bar")
		.data(data)
		.enter().append("rect")
		.attr("class", "bar")
		.attr("x", function(d) { return x(d.size); })
		.attr("width", x.rangeBand())
		.attr("y", function(d) { return y(d.frequency); })
		.attr("height", function(d) { return height - y(d.frequency); })
		.on('mouseover', tip.show)
		.on('mouseout', tip.hide);

	d3.select("#"+domId+" input").on("change", change);

	var sortTimeout = setTimeout(function() {
		d3.select("#"+domId+" input").property("checked", true).each(change);
	}, 500);

	function change() {
		clearTimeout(sortTimeout);

		// Copy-on-write since tweens are evaluated after a delay.
		var x0 = x.domain(data.sort(this.checked
			? function(a, b) { return b.frequency - a.frequency; }
			: function(a, b) { return d3.ascending(a.size, b.size); })
			.map(function(d) { return d.size; }))
			.copy();

		var transition = svg.transition().duration(750),
			delay = function(d, i) { return i * 50; };

		transition.selectAll(".bar")
			.delay(delay)
			.attr("x", function(d) { return x0(d.size); });

		transition.select(".x.axis")
			.call(xAxis)
		.selectAll("g")
			.delay(delay);
	}
}

//init and export
var maltAllocSizeDistr = new MaltPageAllocSizeDistr();
