function MattPageRealloc()
{
	//declare module to manage matt home page
	var mattModule = angular.module('matt.page.realloc',[]);
	var svgId = 0;
	var cur = this;
	
	//main controler of the page
	var pageCtrl = mattCtrl.controller('matt.page.realloc.ctrl',['$scope','$http',function($scope,$http) {
		//fetch summaryData
		$http.get('/realloc-map.json').success(function(data) {
			$scope.reallocMap = data;
			cur.plotMostUsedDelta('matt-most-used-delta',cur.genDeltaDistr(data));
			cur.plotLogDelta('matt-log-delta',cur.genDeltaDistr(data));
			cur.resizeMap2('matt-resize-map',data);
		});
	}]);
}

MattPageRealloc.prototype.genDeltaDistr = function(data)
{
	var res = {};
	for (var i in data)
	{
		if (data[i].newSize != 0 && data[i].oldSize != 0)
		{
			var delta = data[i].newSize - data[i].oldSize;
			if (res[delta] == undefined)
				res[delta] = data[i].count;
			else
				res[delta] += data[i].count;
		}
	}
	return res;
}

MattPageRealloc.prototype.buildLog2Histo = function(data)
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
	
	console.log(hist);
	
	return hist;
}

MattPageRealloc.prototype.plotLogDelta = function(domId,data)
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
		.tickFormat(function(d) { return (d<0?'-':'')+mattHelper.humanReadable(d==0?0:Math.pow(2,Math.abs(d)-1),1,'B',false); });

	var yAxis = d3.svg.axis()
		.scale(y)
		.orient("left")
		.tickFormat(function(d) { return mattHelper.humanReadable(d,1,'',false); });

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
			return "<strong>"+ ((d.sizeLog<0)?'-':'')+mattHelper.humanReadable(d.sizeLog==0?0:Math.pow(2,Math.abs(d.sizeLog)-1),1,'B',false) + " - " + ((d.sizeLog<0)?'-':'')+mattHelper.humanReadable(Math.pow(2,Math.abs(d.sizeLog)),1,'B',false) +":</strong> <span style='color:red'>" + mattHelper.humanReadable(d.count,1,'',false) + "</span>";
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

MattPageRealloc.prototype.plotMostUsedDelta = function(domId,data)
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
		.tickFormat(function(d) { if (d == 0) { return 'others'; } else { if (+d >= 0) return mattHelper.humanReadable(d,1,'',false); else return '-' + mattHelper.humanReadable(-d,1,'',false); } });

	var yAxis = d3.svg.axis()
		.scale(y)
		.orient("left")
		.tickFormat(function(d) { return mattHelper.humanReadable(d,1,'',false); });
		
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
			return "<strong>Frequency:</strong> <span style='color:red'>" + mattHelper.humanReadable(d.frequency,1,'',false) + "</span>";
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
MattPageRealloc.prototype.resizeMap2 = function(domId,data)
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
		linklist.push({source:map[data[i].oldSize],target:map[data[i].newSize]});

// 	d3.json("readme-flare-imports.json", function(error, classes) {
	var nodes = cluster.nodes(nodelist),
		links = linklist;

	link = link
		.data(bundle(links))
		.enter().append("path")
		.each(function(d) { d.source = d[0], d.target = d[d.length - 1]; })
		.attr("class", "link")
// 		.attr("stroke-width",20)
		.attr("d", line);

	node = node
		.data(nodes.filter(function(n) { return !n.children; }))
		.enter().append("text")
		.attr("class", "node")
		.attr("dy", ".31em")
		.attr("transform", function(d) { return "rotate(" + (d.x - 90) + ")translate(" + (d.y + 8) + ",0)" + (d.x < 180 ? "" : "rotate(180)"); })
		.style("text-anchor", function(d) { return d.x < 180 ? "start" : "end"; })
		.text(function(d) { return mattHelper.humanReadable(+d.key,1,'B',false); })
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
			recurse(depth+1);
	}
	var recDepth = recurse(0);
	console.log(recDepth);

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

MattPageRealloc.prototype.resizeMap = function(domId,data)
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
		linklist.push({source:map[data[i].oldSize],target:map[data[i].newSize]});

// 	d3.json("readme-flare-imports.json", function(error, classes) {
	var nodes = cluster.nodes(nodelist),
		links = linklist;

	svg.selectAll(".link")
		.data(bundle(links))
		.enter().append("path")
		.attr("class", "link")
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
		.text(function(d) { return mattHelper.humanReadable(+d.key,1,'B',false); });
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

var mattPageRealloc = new MattPageRealloc();
