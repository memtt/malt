/*****************************************************
             PROJECT  : MALT
             VERSION  : 2.0.0
             DATE     : 07/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/****************************************************/
//deps
var fs    = require('fs');
var clone = require('clone');
var path  = require('path');

/****************************************************/
function MaltProject(file)
{
	//declare internal stats to get a short list in same place
	this.data = null;//store data tree
	this.file = null;//more for debug to remember the related data file

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

		//check version
		if (data.run.formatVersion != 1) {
			console.error("Invalid file formar version. Expect 1 !");
			process.exit(1);
		}

		cur.loadData(data);
	});
}

/****************************************************/
MaltProject.prototype.getRunInfos = function()
{
	return {
		"run": this.data.run,
		"globals": this.data.globals
	};
}

/****************************************************/
function getTimelineMax(data,subtree,entry)
{
	var local = data.timeline;
	if (local == undefined)
		return 0;
	local = local[subtree];
	if (local == undefined)
		return 0;
	local = local[entry];
	if (local == undefined)
		return 0;
	return Math.max.apply(null,local)
}

/****************************************************/
function getMaxRate(data,entry)
{
	//get max
	var max = getTimelineMax(data,"opsBandwidth",entry);
	return (max / data.timeline['opsBandwidth'].steps) * data.globals.ticksPerSecond;
}

/****************************************************/
var allocfuncs = ['malloc','calloc','realloc','memalign','posix_memalign','valloc','pvalloc','aligned_alloc'];
function getThreadSum(data,entry)
{
	var res = 0;
	for (var thread in data.threads)
		for(var func in allocfuncs)
			res += data.threads[thread][allocfuncs[func]][entry];
	return res;
}

/****************************************************/
function getThreadMinMax(data,entry,mode)
{
	var res = -1;
	for (var thread in data.threads) {
		for(var func in allocfuncs) {
			var val = data.threads[thread][allocfuncs[func]][entry];
			if (mode == 'min') {
				if ((val < res || res == -1) && val != 0)
					res = val;
			} else if (mode == 'max') {
				if ((val > res || res == -1) && val != 0)
					res = val;
			}
		}
	}

	return res;
}

/****************************************************/
function getGlobalVarCount(data)
{
	var cnt = 0;
	for (var i in data.globalVariables)
		cnt += data.globalVariables[i].length;
	return cnt;
}

/****************************************************/
function getGlobalVarMem(data,tls)
{
	var ret = 0;
	for (var i in data.globalVariables)
		for (var j in data.globalVariables[i])
			if (data.globalVariables[i][j].tls == tls)
				ret += data.globalVariables[i][j].size;
	return ret;
}

/****************************************************/
MaltProject.prototype.getSummaryInfos = function()
{
	var ret = {};

	//summary
	ret.peakPhysicalMemory = getTimelineMax(this.data,'memStats','processPhysical');
	ret.peakVirtualMemory = getTimelineMax(this.data,'memStats','processVirtual');
	ret.peakRequestedMemory = getTimelineMax(this.data,'memStats','requestedByMalloc');
	ret.peakInternalMemory = getTimelineMax(this.data,'memStats','mattInternalMalloc');
	ret.peakSegmentCount = getTimelineMax(this.data,'memStats','mallocSegments');

	//rates
	ret.peakAllocRate = getMaxRate(this.data,'allocSize');
	ret.peakAllocCountRate = getMaxRate(this.data,'allocCount');

	//sum
	ret.totalAllocatedMemory = getThreadSum(this.data,'sumSize');
	ret.recyclingRatio = ret.totalAllocatedMemory / ret.peakRequestedMemory;
	ret.allocCount = getThreadSum(this.data,'count');
	ret.minAllocSize = getThreadMinMax(this.data,'minSize','min');

	//mean
	ret.meanAllocSize = ret.totalAllocatedMemory / ret.allocCount;

	//min/max
	ret.minAllocSize = getThreadMinMax(this.data,"minSize","min");
	ret.maxAllocSize = getThreadMinMax(this.data,"maxSize","max");

	//leaks
	var count = 0;
	var mem = 0;
	for (var i in this.data.leaks)
	{
		count += this.data.leaks[i].count;
		mem += this.data.leaks[i].mem;
	}
	this.leakedMem = mem;
	this.leakedCount = count;

	//stacks TODO

	//global variables
	ret.numGblVar = getGlobalVarCount(this.data);
	ret.globalVarMem = getGlobalVarMem(this.data,false);
	ret.tlsVarMem = getGlobalVarMem(this.data,true);

	//ok return
	return ret;
}

/****************************************************/
//export definition
module.exports = MaltProject;
