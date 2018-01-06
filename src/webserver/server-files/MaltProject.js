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
		cur.reshape();
	});
}

/****************************************************/
MaltProject.prototype.reshapeTimelineZeroRef = function()
{
	var keys = ["memStats","systemStats","opsBandwidth"];
	for (var i in this.data.timeline) {
		var entry = this.data.timeline[i];
		for (var j in entry) {
			if (j != "steps")
				entry[j].push(0);
		}
	}
}

/****************************************************/
MaltProject.prototype.reshapeAddrMap = function()
{
	this.addrMap = {};
	var map = this.data.stacks.tree.addrMap;
	for (var i in map)
		this.addrMap[map[i]] = i;
}

/****************************************************/
MaltProject.prototype.reshapeStackTime = function()
{
	for (var i in this.data.threads) {
		this.data.threads[i].stackSize.stackIds.push(0);
		this.data.threads[i].stackSize.size.push(0);
	}
}

/****************************************************/
MaltProject.prototype.reshapeBuildStackListRecurse = function(stack,pointer)
{
	//check if add content
	if (pointer.dataId != undefined) {
		this.stacksByDataId[pointer.dataId] = clone(stack);
	}

	var pos = stack.length;
	stack.push(0);
	for (var i in pointer) {
		if (i != "dataId") {
			stack[pos] = i;
			this.reshapeBuildStackListRecurse(stack,pointer[i]);
		}
	}
	stack.pop();
}

/****************************************************/
MaltProject.prototype.reshapeBuildStackList = function()
{
	this.stacksByDataId = {};
	var stack = [];
	this.reshapeBuildStackListRecurse(stack,this.data.stacks.tree.calltree);
}

/****************************************************/
MaltProject.prototype.reshapeInstrStrings = function()
{
	var strings = this.data.symbols.strings;
	for (var i in this.data.symbols.instr) {
		var sym = this.data.symbols.instr[i];
		sym.file = strings[sym.file];
		sym.function = strings[sym.function];
		sym.binary = strings[sym.binary];
	}
}

/****************************************************/
MaltProject.prototype.reshape = function()
{
	this.reshapeTimelineZeroRef();
	this.reshapeStackTime();
	this.reshapeAddrMap();
	this.reshapeBuildStackList();
	this.reshapeInstrStrings();
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
			res += data.threads[thread].stats[allocfuncs[func]][entry];
	return res;
}

/****************************************************/
function getThreadMinMax(data,entry,mode)
{
	var res = -1;
	for (var thread in data.threads) {
		for(var func in allocfuncs) {
			var val = data.threads[thread].stats[allocfuncs[func]][entry];
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
function getLargestStack(data)
{
	var max = 0;
	for (var i in data.threads)
	{
		var tmp = Math.max.apply(null,data.threads[i].stackSize.size);
		max = Math.max(max,tmp);
	}
	return max;
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
	ret.leakedMem = mem;
	ret.leakedCount = count;

	//stacks
	ret.largestStack = getLargestStack(this.data);

	//global variables
	ret.numGblVar = getGlobalVarCount(this.data);
	ret.globalVarMem = getGlobalVarMem(this.data,false);
	ret.tlsVarMem = getGlobalVarMem(this.data,true);

	//ok return
	return ret;
}

/****************************************************/
MaltProject.prototype.getStackSize = function()
{
	var out = [];
	var max = [];
	for (var i in this.data.threads) {
		var thread = this.data.threads[i];
		out.push(thread.stackSize);
		max.push(thread.max);
	}
	return {
		data:out,
		max:max,
		ticksPerSecond: this.data.globals.ticksPerSecond
	};
}

/****************************************************/
MaltProject.prototype.getAllocSizeDistr = function()
{
	return this.data.allocSizeDistr;
}

/****************************************************/
MaltProject.prototype.getStackSizeDetails = function(threadId)
{
	//get data
	var mem = this.data.threads[threadId].max.mem;
	var stack = this.genDetailedStack(this.data.threads[threadId].max.stack);

	//reshape
	var out = {};
	for (var i in mem) {
		if (stack[i] != undefined) {
			if (out[stack[i].function] == undefined) {
				out[stack[i].function] = {
					location:stack[i],
					mem:+mem[i]
				}
			} else {
				out[stack[i].function].mem += mem[i];
			}
		}
	}

	//build array
	var outFinal = [];
	for (var i in out)
		outFinal.push(out[i]);

	//sort
	outFinal.sort(function(a,b){a.mem - b.mem});

	return outFinal;
}

/****************************************************/
MaltProject.prototype.genDetailedStack = function(stack) {
	var out = [];
	stack.forEach( el => {
		out.push(this.data.symbols.instr[el]);
	})
	return out;
}

/****************************************************/
MaltProject.prototype.getThreadStats = function()
{
	var ret = [];
	for (var i in this.data.threads)
		ret.push(this.data.threads[i].stats);
	return {
		data:ret,
		ticksPerSecond: this.data.globals.ticksPerSecond
	};
}

/****************************************************/
MaltProject.prototype.getTimeline = function()
{
	return {
		data:this.data.timeline,
		ticksPerSecond: this.data.globals.ticksPerSecond
	};
}

/****************************************************/
//export definition
module.exports = MaltProject;
