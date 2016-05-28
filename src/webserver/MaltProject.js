/****************************************************/
//deps
var fs    = require('fs');
var clone = require('clone');
var path  = require('path');

/****************************************************/
function MaltProject(file)
{
	this.file = file;
	
	//load file
	if (file != undefined)
	{
		console.log("loading file "+file+"...");
		this.loadFile(file);
	}
}

/****************************************************/
MaltProject.prototype.loadData = function(data)
{
	//setup current data
	this.data = data;
	
// 	this.data.stacks = this.getFullTree();
// 	console.log(JSON.stringify(data));
// 	this.data = null;

	//optimize data
	console.log("Optimizing datas for requests...");
// 	optimizeProjectDatas(data);
	console.log("Data optimization done.");
}

/****************************************************/
/**
 * Function in charge of loading the json file.
**/
MaltProject.prototype.loadFile = function(file)
{
	//init
	this.data = null;
	this.file = file;
	var cur = this;
	
	//read from FS
	fs.readFile(args.params.input, 'utf8', function (err, buffer) {
		console.log("Loading file : " + args.params.input);

		//manage errors
		if (err) {
			console.error("Failed to open file "+file+", get error : "+err.code);
			process.exit(1);
// 			console.log(err);
// 			throw new Error(err);
		}
		
		//ok parse
		var data = JSON.parse(buffer);
		
		cur.loadData(data);
	});
}

/****************************************************/
/**
 * Build a summary from the whole datas.
**/
MaltProject.prototype.getSummaryV2 = function()
{
	var ret = {};
	
	//extract run info
	ret.run = this.data.run;
	
// 	//extract system info
// 	ret.system = {};
// 	ret.system.totalMemory = this.data.globals.totalMemory
// 	ret.system.ticksPerSecond = this.data.globals.ticksPerSecond;
// 	
// 	//summary
// 	ret.summary = {};
// 	ret.summary.peakPhysicalMemory = this.data.timeline.memoryTimeline.peak[2];//this.data.timeline.physicalMem.peakMemory;
// 	ret.summary.peakVirtualMemory = this.data.timeline.memoryTimeline.peak[1];//this.data.timeline.virtualMem.peakMemory;
// 	ret.summary.peakRequestedMemory = this.data.timeline.memoryTimeline.peak[0];//this.data.timeline.requestedMem.peakMemory;
// 	ret.summary.peakInternalMemory = this.data.timeline.memoryTimeline.peak[3];//this.data.timeline.internalMem.peakMemory;
// 	ret.summary.peakSegmentCount = this.data.timeline.memoryTimeline.peak[4];//this.data.timeline.segments.peakMemory;
// 	
// 	//rates
// 	var p = 0;
// 	for (var i in this.data.timeline.memoryBandwidth.values)
// 		if (this.data.timeline.memoryBandwidth.values[i][1] > p)
// 			p = this.data.timeline.memoryBandwidth.values[i][1];
// 	ret.summary.peakAllocRate = (p / this.data.timeline.memoryBandwidth.perPoints) * this.data.globals.ticksPerSecond;
// 	
// 	p = 0;
// 	for (var i in this.data.timeline.memoryBandwidth.values)
// 		if (this.data.timeline.memoryBandwidth.values[i][3] > p)
// 			p = this.data.timeline.memoryBandwidth.values[i][3];
// 	ret.summary.peakAllocCountRate = (p / this.data.timeline.memoryBandwidth.perPoints) * this.data.globals.ticksPerSecond;
// 
// 	//search min/max/count size
// 	var min = -1;
// 	var max = -1;
// 	var count = 0;
// 	var stats = this.data.stacks.stats;
// 	var sum = 0;
// 	for(var i in stats)
// 	{
// 		var info = stats[i].infos;
// 		if ((info.alloc.min < min || min == -1) && info.alloc.min > 0)
// 			min = info.alloc.min;
// 		if (info.alloc.max > max || max == -1)
// 			max = info.alloc.max;
// 		count += info.alloc.count;
// 		sum += info.alloc.sum;
// 	}
// 	
// 	//gen
// 	ret.summary.totalAllocatedMemory = sum;
// 	ret.summary.recyclingRatio = sum / ret.summary.peakRequestedMemory;
// 	ret.summary.allocCount = count;
// 	ret.summary.minAllocSize = min;
// 	ret.summary.meanAllocSize = sum / count;
// 	ret.summary.maxAllocSize = max;
// 	
// 	//leaks
// 	var leakCount = 0;
// 	var leakMem = 0;
// 	var leaks = this.data.leaks;
// 	for (var i in leaks)
// 	{
// 		leakCount += leaks[i].count;
// 		leakMem += leaks[i].memory;
// 	}
// 	ret.summary.leakedMem = leakMem;
// 	ret.summary.leakedCount = leakCount;
// 	
// 	//stacks
// 	ret.summary.largestStack = this.getMaxStack().size;
// 	
// 	//global vars
// 	var tlsMem = 0;
// 	var gblMem = 0;
// 	var cntVars = 0;
// 	var gvars = this.data.memStats.globalVariables;
// 	for (var i in gvars)
// 	{
// 		for (var j in gvars[i])
// 		{
// 			cntVars++;
// 			if (gvars[i][j].tls)
// 				tlsMem += gvars[i][j].size;
// 			else
// 				gblMem += gvars[i][j].size;
// 		}
// 	}
// 	ret.summary.numGblVar = cntVars;
// 	ret.summary.globalVarMem = gblMem;
// 	ret.summary.tlsVarMem = tlsMem;
// 	
// 	//summary warnings
// 	ret.summaryWarnings = this.genSummaryWarnings(ret);
// 	
// 	//thread stats
// 	ret.threadStats = [];
// 	for (var i in this.data.threads)
// 		ret.threadStats.push(this.data.threads[i].stats);
// 
// 	//return
	return ret;
}

/****************************************************/
module.exports = MaltProject;
