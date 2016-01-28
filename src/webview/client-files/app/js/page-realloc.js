function MaltPageRealloc()
{
	//declare module to manage malt home page
	var maltModule = angular.module('malt.page.realloc',[]);
	var svgId = 0;
	var cur = this;
	
	//main controler of the page
	var pageCtrl = maltCtrl.controller('malt.page.realloc.ctrl',['$scope','$http',function($scope,$http) {
		//fetch summaryData
		maltDataSource.loadReallocStats($http,function(data) {
			$scope.reallocMap = data;
			
			//search most used
			var mostUsed = data[0];
			var meanDelta = 0;
			var count = 0;
			for (var i in data)
			{
				meanDelta += Math.abs(data[i].oldSize - data[i].newSize);
				count += data[i].count;
				if (data[i].count > mostUsed.count)
					mostUsed = data[i];
			}
			
			$scope.realloCount = maltHelper.humanReadable(count,1,"",false);
			$scope.meanDelta = maltHelper.humanReadable(meanDelta/data.length,1,"B",false);
			$scope.formatedMostUsed = maltHelper.humanReadable(mostUsed.count,1,"",false) + " of " + maltHelper.humanReadable(mostUsed.oldSize,1,"B",false) + " -> " + maltHelper.humanReadable(mostUsed.newSize,1,"B",false);
			
			cur.plotMostUsed('malt-most-used',data,20);
			cur.plotMostUsedDelta('malt-most-used-delta',cur.genDeltaDistr(data));
			cur.plotLogDelta('malt-log-delta',cur.genDeltaDistr(data));
			cur.resizeMap2('malt-resize-map',data);
			cur.plotXY('malt-xy',data);
		});
	}]);
}

MaltPageRealloc.prototype.genXYData = function(data)
{
	var ret = [];
	
	ret.push({
      key: 'Realloc',
      values: []
    });
	
	for (var i in data)
	{
		ret[0].values.push({
			x: data[i].newSize,
			y: data[i].oldSize,
			size: 2*Math.log(data[i].count),
			shape:'circle'
		});
	}
	return ret;
}

MaltPageRealloc.prototype.plotXY = function(domId,data)
{
	var cur = this;
	nv.addGraph(function() {
		var chart = nv.models.scatterChart()
						.showDistX(true)    //showDist, when true, will display those little distribution lines on the axis.
						.showDistY(true)
// 						.transitionDuration(350)
						.color(d3.scale.category10().range());

		//Configure how the tooltip looks.
// 		chart.tooltipContent(function(key,a,b,c) {
// 			console.log(c);
// 			return key+" : "+b+ " -> "+a;
// 		});

		//Axis settings
		chart.xAxis
			.axisLabel('Old size')
			.tickFormat(function(d) { return maltHelper.humanReadable(d,1,'B',false); });
		chart.yAxis
			.axisLabel('New size')
			.tickFormat(function(d) { return maltHelper.humanReadable(d,1,'B',false); });
// 		chart.scatter.onlyCircles(true);

		d3.select('#'+domId+' svg')
			.datum(cur.genXYData(data))
			.call(chart);

		nv.utils.windowResize(chart.update);

		return chart;
		});
}

MaltPageRealloc.prototype.plotMostUsed = function(domId,data,maxNb)
{
	var data = this.cutMostUsed(data,maxNb);
	
	nv.addGraph(function() {

		var chart = nv.models.multiBarChart()
			.x(function(d) { return d.name })
			.y(function(d) { return d.count })
// 			.margin({top: 30, right: 20, bottom: 50, left: 175})
// 			.showValues(true)           //Show bar value next to each bar.
// 			.tooltips(true)             //Show tooltips on hover.
// 			.transitionDuration(350)
			.showControls(false)
			.reduceXTicks(false)
			.stacked(true);        //Allow user to switch between "Grouped" and "Stacked" mode.
// 			.tooltipContent(function(serieName,name,value,e,graph) {
// 				var d = e.series.values[e.pointIndex];
// 				var pos = "";
// 				var tls = "";
// 				if (d.file != undefined && d.file != '')
// 					pos = "<br/>" + d.file + ":" + d.line;
// 				if (e.series.key == "TLS variables")
// 					tls = " [ "+e.series.tlsInstances+" * "+maltHelper.humanReadable(d.value/e.series.tlsInstances,1,'B',false) +" ] ";
// 				var ratio = " ( "+(100*d.value/e.series.total).toFixed(2)+"% ) ";
// 				console.log(data);
// 				console.log(e);
// 				return "<div style='text-align:center'><h3>"+d.name+"</h3>"+maltHelper.humanReadable(d.value,1,'B',false)+tls+ratio+pos+'</div>';
// 			});

// 		$scope.chart = chart;
		chart.yAxis
			.tickFormat(function(d) {return maltHelper.humanReadable(d,1,"",false);});
			
		d3.select("#"+domId + " svg")
			.datum(data)
			.call(chart);
		
		nv.utils.windowResize(chart.update);

		return chart;
	});
}

MaltPageRealloc.prototype.cutMostUsed = function(data,maxNb)
{
	var sortedData = data.sort(function(a,b) {return b.count - a.count;});
	
	var others = 0;
	var cnt = 0;
	var ret = [];
	for (var i in sortedData)
	{
		cnt++;
		if (cnt <= maxNb)
		{
			//console.log(cnt + " -> " + sortedData[i].oldSize + " -> " + sortedData[i].newSize + " -> "+sortedData[i].count);
			ret.push({name:maltHelper.humanReadable(sortedData[i].oldSize,1,'B',false) + " -> " + maltHelper.humanReadable(sortedData[i].newSize,1,'B',false), count:sortedData[i].count});
		} else {
			others += sortedData[i].count;
		}
	}
	if (others > 0)
		ret.push({name:'others',count:others});
	
	ret = ret.sort(function(a,b) {return b.count - a.count;});

	return [ { key:'Realloc', color:'blue', values:ret } ];
}

MaltPageRealloc.prototype.genDeltaDistr = function(data)
{
	var res = {};
	for (var i in data)
	{
		if (data[i].newSize != 0 && data[i].oldSize != 0)
		{
			var delta = data[i].newSize - data[i].oldSize;
			if (delta != 0)
			{
				if (res[delta] == undefined)
					res[delta] = data[i].count;
				else
					res[delta] += data[i].count;
			}
		}
	}
	return res;
}

MaltPageRealloc.prototype.buildLog2Histo = function(data)
{
	var tmp = {};
	
	for (var i in data)
	{
		var id = Math.ceil(Math.log(Math.abs(+i)) / Math.log(2));
		if (+i < 0)
			id = -id;
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

MaltPageRealloc.prototype.plotLogDelta = function(domId,data)
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
		.tickFormat(function(d) { return (d<0?'-':'')+maltHelper.humanReadable(d==0?0:Math.pow(2,Math.abs(d)-1),1,'B',false); });

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
			return "<strong>"+ ((d.sizeLog<0)?'-':'')+maltHelper.humanReadable(d.sizeLog==0?0:Math.pow(2,Math.abs(d.sizeLog)-1),1,'B',false) + " - " + ((d.sizeLog<0)?'-':'')+maltHelper.humanReadable(Math.pow(2,Math.abs(d.sizeLog)),1,'B',false) +":</strong> <span style='color:red'>" + maltHelper.humanReadable(d.count,1,'',false) + "</span>";
		})

	svg.call(tip);

	x.domain([d3.min(hist, function(d) { return d.sizeLog; })-1, 1+d3.max(hist, function(d) { return d.sizeLog; })]);
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
		.attr("x", function(d) { return d.sizeLog >=0 ? x(d.sizeLog) : x(d.sizeLog - 1); })
		.attr("width", x(1) - x(0))
		.attr("y", function(d) { return y(d.count); })
		.attr("height", function(d) { return height - y(d.count); })
		.on('mouseover', tip.show)
		.on('mouseout', tip.hide);
}

MaltPageRealloc.prototype.plotMostUsedDelta = function(domId,data)
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
		.tickFormat(function(d) { if (d == 0) { return 'others'; } else { if (+d >= 0) return maltHelper.humanReadable(d,1,'',false); else return '-' + maltHelper.humanReadable(-d,1,'',false); } });

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

//http://bl.ocks.org/mbostock/1044242
//http://bl.ocks.org/mbostock/7607999
//http://bl.ocks.org/mbostock/4341134
MaltPageRealloc.prototype.resizeMap2 = function(domId,data)
{
	var diameter = 960,
		radius = diameter / 2,
		innerRadius = radius - 120;

	var cluster = d3.layout.cluster()
		.size([360, innerRadius])
		.sort(null)
		.value(function(d) { return d.size; });

	var bundle = d3.layout.bundle();

	var line = d3.svg.line.radial()
		.interpolate("bundle")
		.tension(.85)
		.radius(function(d) { return d.y; })
		.angle(function(d) { return d.x / 180 * Math.PI; });

	var svg = d3.select("#"+domId+" svg")
		.attr("width", diameter)
		.attr("height", diameter)
	.append("g")
		.attr("transform", "translate(" + radius + "," + radius + ")");

	var link = svg.append("g").selectAll(".link"),
		node = svg.append("g").selectAll(".node");

	var tmp = {};
	var linklist = [];
	var max = 0;
	for (var i in data)
	{
		tmp[data[i].newSize] = true;
		tmp[data[i].oldSize] = true;
		if (data[i].count > max)
			max = data[i].count;
	}
	var nodelist = {name:'',children:[]};
	var map = {};
	for (var i in tmp)
	{
		var t = map[i] = {name:i,children:[],parent:nodelist,key:i};
		nodelist.children.push(t);
	}
	
	for (var i in data)
		linklist.push({source:map[data[i].oldSize],target:map[data[i].newSize],count:data[i].count});

// 	d3.json("readme-flare-imports.json", function(error, classes) {
	var nodes = cluster.nodes(nodelist),
		links = linklist;

	link = link
		.data(bundle(links))
		.enter().append("path")
		.each(function(d) { d.source = d[0], d.target = d[d.length - 1]; })
		.attr("class", "link")
// 		.attr("stroke-width",function(d) { console.log(d,i); return "20px"; })
		.attr("d", line);

	node = node
		.data(nodes.filter(function(n) { return !n.children; }))
		.enter().append("text")
		.attr("class", "node")
		.attr("dy", ".31em")
		.attr("transform", function(d) { return "rotate(" + (d.x - 90) + ")translate(" + (d.y + 8) + ",0)" + (d.x < 180 ? "" : "rotate(180)"); })
		.style("text-anchor", function(d) { return d.x < 180 ? "start" : "end"; })
		.text(function(d) { return maltHelper.humanReadable(+d.key,1,'B',false); })
		.on("mouseover", mouseovered)
		.on("mouseout", mouseouted);
// 	});

	function mouseovered(d) {
		node
			.each(function(n) { n.target = n.source = false; });

		link
			.classed("link--target", function(l) { if (l.target === d) return l.source.source = true; })
			.classed("link--source", function(l) { if (l.source === d) return l.target.target = true; })
			.filter(function(l) { return l.target === d || l.source === d; })
			.each(function() { this.parentNode.appendChild(this); });
			
		function recurse(depth) {
			var cnt = 0;
			link
				.classed("link--target", function(l) { if (l.target.source && !l.source.source) { cnt++; return l.source.source = true; } else { return l.source.source; } })
				.classed("link--source", function(l) { if (l.source.target && !l.target.target) { cnt++; return l.target.target = true; } else { return l.target.target; } })
	// 			.classed("link--source", function(l) { if (l.source === u && mode == 'source' && (l.target.target != true)) { recurse(l.target,"target"); return l.target.target = true; } })
	// 			.filter(function(l) { return l[mode] === u; })
				.each(function() { this.parentNode.appendChild(this); });
			if (cnt > 0)
				return recurse(depth+1);
			else
				return depth;
		}
		
		var doRecurse = false;
		d3.select("#"+domId+" input").each(function() {doRecurse = this.checked;});
		if (doRecurse)
			var recDepth = recurse(0);

		node
			.classed("node--target", function(n) { return n.target; })
			.classed("node--source", function(n) { return n.source; });
	}

	function mouseouted(d) {
	link
		.classed("link--target", false)
		.classed("link--source", false);

	node
		.classed("node--target", false)
		.classed("node--source", false);
	}

	d3.select(self.frameElement).style("height", diameter + "px");
}

MaltPageRealloc.prototype.resizeMap = function(domId,data)
{
// 	data = [
// 		{
// 			oldSize:1,
// 			newSize:10,
// 			count:100
// 		},
// 		{
// 			oldSize:1,
// 			newSize:20,
// 			count:100
// 		},
// 		{
// 			oldSize:20,
// 			newSize:30,
// 			count:100
// 		}
// 	];
	
	var diameter = 960,
		radius = diameter / 2,
		innerRadius = radius - 220;

	var cluster = d3.layout.cluster()
		.size([360, innerRadius])
		.sort(null)
		.value(function(d) { return d.size; });

	var bundle = d3.layout.bundle();

	var line = d3.svg.line.radial()
		.interpolate("bundle")
		.tension(.85)
		.radius(function(d) { return d.y; })
		.angle(function(d) { return d.x / 180 * Math.PI; });

	var svg = d3.select("body").append("svg")
		.attr("width", diameter)
		.attr("height", diameter)
	.append("g")
		.attr("transform", "translate(" + radius + "," + radius + ")");
		
	var tmp = {};
	var linklist = [];
	for (var i in data)
	{
		tmp[data[i].newSize] = true;
		tmp[data[i].oldSize] = true;
	}
	var nodelist = {name:'',children:[]};
	var map = {};
	for (var i in tmp)
	{
		var t = map[i] = {name:i,children:[],parent:nodelist,key:i};
		nodelist.children.push(t);
	}
	
	for (var i in data)
		linklist.push({source:map[data[i].oldSize],target:map[data[i].newSize],count:data[i].count});

// 	d3.json("readme-flare-imports.json", function(error, classes) {
	var nodes = cluster.nodes(nodelist),
		links = linklist;

	svg.selectAll(".link")
		.data(bundle(links))
		.enter().append("path")
		.attr("class", "link")
		.attr("stroke-width", 20)
		.attr("d", line);

	svg.selectAll(".node")
		.data(nodes.filter(function(n) { return !n.children; }))
		.enter().append("g")
		.attr("class", "node")
		.attr("transform", function(d) { return "rotate(" + (d.x - 90) + ")translate(" + d.y + ")"; })
		.append("text")
		.attr("dx", function(d) { return d.x < 180 ? 8 : -8; })
		.attr("dy", ".31em")
		.attr("text-anchor", function(d) { return d.x < 180 ? "start" : "end"; })
		.attr("transform", function(d) { return d.x < 180 ? null : "rotate(180)"; })
		.text(function(d) { return maltHelper.humanReadable(+d.key,1,'B',false); });
// 	});

	d3.select(self.frameElement).style("height", diameter + "px");

	// Lazily construct the package hierarchy from class names.
	function packageHierarchy(classes) {
	var map = {};

	function find(name, data) {
		var node = map[name], i;
		if (!node) {
		node = map[name] = data || {name: name, children: []};
		if (name.length) {
			node.parent = find(name.substring(0, i = name.lastIndexOf(".")));
			node.parent.children.push(node);
			node.key = name.substring(i + 1);
		}
		}
		return node;
	}

	classes.forEach(function(d) {
		find(d.name, d);
	});

	return map[""];
	}

	// Return a list of imports for the given array of nodes.
	function packageImports(nodes) {
	var map = {},
		imports = [];

	// Compute a map from name to node.
	nodes.forEach(function(d) {
		map[d.name] = d;
	});

	// For each import, construct a link from the source to target node.
	nodes.forEach(function(d) {
		if (d.imports) d.imports.forEach(function(i) {
		imports.push({source: map[d.name], target: map[i]});
		});
	});

	return imports;
	}
}

var maltPageRealloc = new MaltPageRealloc();
