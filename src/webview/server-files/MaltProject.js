/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/server-files/MaltProject.js
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
*    AUTHOR   : Sébastien Valat - 2015 - 2019
*    AUTHOR   : Mehdi Raza Jaffery (CERN) - 2016
*    AUTHOR   : Sébastien Valat (INRIA) - 2023 - 2025
*    AUTHOR   : Antoine Bernard (crans.org) - 2024
***********************************************************/

/**********************************************************/
//deps
var fs    = require('fs');
var json  = require('big-json');
var clone = require('clone');
var path  = require('path');
var CallTreeAdapter = require("./CallTreeAdapter.js");
var GraphGenerator = require("./GraphGenerator.js");
var CppDeclParser = require("./CppDeclParser.js");
var childProcess = require('child_process');
const cliProgress = require('cli-progress');
const colors = require('ansi-colors');

/**********************************************************/
/**
 * Construct a MaltProject by loading data in JSON format from given file.
**/
//--PORTED IN C++ VERSION--
function MaltProject(file, callback = function(){})
{
	//declare internal stats to get a short list in same place
	this.data = null;//store data tree
	this.file = null;//more for debug to remember the related data file

	//load file
	if (file != undefined)
	{
		console.log("loading file "+file+"...");
		this.loadFile(file, callback);
	}
}

/**********************************************************/
//--PORTED IN C++ VERSION--
MaltProject.prototype.loadData = function(data)
{
	//check version compatibility
	if (data.run.formatVersion != "1.1") {
		console.error("!!!!!!!!!!!!!!!!!!!!!!!  WARNING !!!!!!!!!!!!!!!!!!!!!!!!");
		console.error("");
		console.error("Invalid JSON format version detected, your profile might have been generated by an \
					   older/newer MALT backend version ("+data.run.tool+").");
		console.error("Try to continue to not block, but issues are expected into the GUI !!!!!")
		console.error("");
		console.error("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	}

	//setup current data
	this.data = data;

	//this is more to debug
// 	this.data.stacks = this.getFullTree();
// 	console.log(JSON.stringify(data));
// 	this.data = null;

	//optimize data
	console.log("Optimizing datas for requests...");
	optimizeProjectDatas(data);
	console.log("Data optimization done.");
}

/**********************************************************/
/**
 * Function in charge of loading the json file.
**/
MaltProject.prototype.loadFile = function(file, callback = function(){})
{
	//init
	this.data = null;
	this.file = file;
	var cur = this;

	//big
	let { size } = fs.statSync(args.params.input);
	if (size > 500*1024*1024) {
		console.log(`Profile file is ${Math.round(size/1024.0/1024.0)} MB it may take a LONG TIME to load !`);
	}

	// create a read and a json-parser stream
	const readStream = fs.createReadStream(args.params.input, { encoding: 'utf8' });
	const parseStream = json.createParseStream();

	// receive parsed JSON, and load it
	let written = 0;
	parseStream.on('data', function(data) {
		cur.loadData(data);
		callback();
	});

	// handle end of file
	parseStream.on('end', () => {
		console.log('Finished processing JSON file');
	});
	readStream.on('end', () => {
		console.log('Finished reading JSON file');
	});

	// handle errors
	parseStream.on('error', (err) => {
		console.error("Failed to parse file "+file+", get error : "+err.message);
		process.exit(1);
	//	console.log(err);
	//	throw new Error(err);
	});
	readStream.on('error', (err) => {
		console.error("Failed to read file "+file+", get error : "+err.message);
		process.exit(1);
	});

	// create new progress bar
	const b1 = new cliProgress.SingleBar({
		format: 'Loading Progress |' + colors.gray('{bar}') + '| {percentage}% || {value}/{total} MB',
		hideCursor: true
	});
	b1.start(Math.round(size/1024/1024), 0, {
		speed: "N/A"
	});

	// handle start of the stream
	let loaded = 0;
	readStream.on('data', (data) => {
		loaded += data.length;
		b1.update(Math.round(loaded/1024/1024));
	});

	// handle start of the stream
	readStream.on('resume', () => {
		//console.log("Loading file : " + args.params.input);
	});

	// handle start of the stream
	readStream.on('end', () => {
		//console.log("Loading file : " + args.params.input);
		b1.stop();
	});

	// Start reading
	readStream.pipe(parseStream);
}

/**********************************************************/
//--PORTED IN C++ VERSION--
MaltProject.prototype.getProcMap = function()
{
	return this.stacks.sites.map;
}

/**********************************************************/
/**
 * Add info about stack to data extracted from traces
**/
MaltProject.prototype.completeMemtraceAt = function(data)
{
	var stats = this.data.stacks.stats;
	var out = [];

	for(var i in stats)
	{
		//extract some short refs
		var statsEntry = stats[i];
		var stack = statsEntry.stack;

		console.log(statsEntry.stackId + " -> " + data[statsEntry.stackId]);
		if (data[statsEntry.stackId] != undefined)
			out.push({stack:statsEntry.detailedStack,info:data[statsEntry.stackId]});
	}
	return out;
}

/**********************************************************/
/**
 * Just to get the trace filename if available.
**/
MaltProject.prototype.getTraceFilename = function()
{
	var ret = this.data.run.tracefile;
	if (ret == undefined)
	{
		//TODO add help by giving option
		console.log("You request usage of trace file but analysis was run without this mode, please re-run your job to get a trace file. !");
		return undefined;
	} else {
		if (this.file != undefined && this.file != "")
		{
			var dir = path.dirname(this.file);
			ret = path.join(dir,ret);
		}
	}
	return ret;
}

/**********************************************************/
/**
 * Just for debug, print only stack with function names.
**/
//--PORTED IN C++ VERSION--
MaltProject.prototype.getDebugStackList = function()
{
	//setup some local vars
	var stats = this.data.stacks.stats;
	var res = [];

	for(var i in stats)
	{
		var statsEntry = stats[i];
		var detailedStack = statsEntry.detailedStack;
		var stack = [];
		for (var j in detailedStack)
		{
			stack.push(detailedStack[j].function);
		}
		res.push(stack);
	}

	return res;
}

/**********************************************************/
/**
 * Provide access to the list of global variables from executable and dynamic libs.
**/
//--PORTED IN C++ VERSION--
MaltProject.prototype.getGlobalVariables = function()
{
	return {
		vars: this.data.memStats.globalVariables,
		maxThreadCount: this.data.globals.maxThreadCount
	};
}

/**********************************************************/
/**
 * Produce a flat profile by projecting stats onto sumbols. You can get some simple examples by going to getFileLinesFlatProfile() or getFunctionFlatProfile()
 * @param mapping Provide a function whith prototype function(entry,info) which return one of the entry field 
 * to be used as fusion criteria. Entry correspond to entries from stacks.stats
 * @param accept Can be 'true' or a function with prototype(entry,info) with entry from stacks.stats[].detailedStack to accept (true) or reject (false) them.
 * @param total If 'true', the output contain 'own' and 'total' otherwise it contain 'own' and 'childs'.
**/
//--PORTED IN C++ VERSION--
MaltProject.prototype.getFlatProfile = function(mapping,accept,fields,total)
{
	//setup some local vars
	var stats = this.data.stacks.stats;
	var res = new Object();
	var callers = "total";
	var cur = null;
	if (total == false)
		callers = "childs";

	for(var i in stats)
	{
		//extract some short refs
		var statsEntry = stats[i];
		var detailedStack = statsEntry.detailedStack;
		var infos = statsEntry.infos;
		var stack = statsEntry.stack;

		//skip C++ operators
		var skip = 0;
		while (skip < detailedStack.length && isAllocFunction(this.data, detailedStack[skip].function)) skip++;
		if (skip >= detailedStack.length)
		{
			console.log("Warning : get call stacks with only allocation function ??? : "+JSON.stringify(detailedStack) +" -> "+JSON.stringify(statsEntry));
			continue;
		}

		//update internal values
		cur = detailedStack[skip];
		if (accept == true || accept(cur,infos) == true)
			mergeStackInfo(res,cur,stack[skip],"own",infos,mapping,fields);

		//childs
		var done = new Object;
		for (var j in stack)
		{
			//skip firsts for 'own' mode, otherwise keep them
			if (total == false && j <= skip)
				continue;

			//extract some quick refs
			cur = detailedStack[j];
			var key = mapping(cur);
			var filter = (accept == true || accept(cur,infos) == true);
			if (filter && done[key] == undefined && !isAllocFunction(this.data, cur.function))
			{
				done[key] = true;
				mergeStackInfo(res,cur,stack[j],callers,infos,mapping,fields);
			}
		}
	}

	//convert to simple list
	var finalRes = new Array();
	for (var i in res)
		finalRes.push(res[i]);

	//ok return
	return finalRes;
}

/**********************************************************/
/**
 * Map memory informations from stack onto file lines.
 * @param total If 'true', produce 'own' and 'total', otherwise produce 'own' and 'childs'.
**/
//--PORTED IN C++ VERSION--
MaltProject.prototype.getFileLinesFlatProfile = function(file,total)
{
	var res = this.getFlatProfile(
		function(entry) {return entry.line;},        //map on lines
		function(entry) {return entry.file == file}, //accept only requested file
		['line','function'],                         //export only line info
		total);
	return res;
}

/**********************************************************/
/**
 * Map memory informations from stack on functions (symbols).
 * @param total If 'true', produce 'own' and 'total', otherwise produce 'own' and 'childs'.
**/
//--PORTED IN C++ VERSION--
MaltProject.prototype.getFlatFunctionProfile = function(total)
{
	var res = this.getFlatProfile(
		function(entry) {return entry.file + ":"+entry.function;},    //map on lines
		true,                                        //accept all
		['function','line','file'],                  //export only line info
		total);
	for (var i = res.length - 1; i >= 0; i--) {
		res[i].functionShort = CppDeclParser.getShortName(CppDeclParser.parseCppPrototype(res[i].function));
	}
	return res;
}

/**********************************************************/
/**
 * Return virtual memory distribution extracted from /proc/self/maps and execution end.
**/
//--PORTED IN C++ VERSION--
MaltProject.prototype.getProcMapDistr = function()
{
	//some local vars
	var res = new Object();
	var map = this.data.sites.map;
	var checkStack = /^\[stack:[0-9]+\]$/;

	//loop on map entries
	for (var i in map)
	{
		//compte mem
		var mem = (parseInt(map[i].upper,16) - parseInt(map[i].lower,16));

		//check goal
		var file = map[i].file;
		if (file == '')
			file = 'anonymous';
		if (checkStack.test(file))
			file = 'stack';

		//sum
		if (res[file] == undefined)
		{
			res[file] = {
				mem:mem,
				cnt:1
			};
		} else {
			res[file].mem += mem;
			res[file].cnt++;
		}
	}

	//ok return
	return res;
}

/**********************************************************/
//--PORTED IN C++ VERSION--
MaltProject.prototype.getScatter = function()
{
	return this.data.scatter;
}

/**********************************************************/
//--PORTED IN C++ VERSION--
MaltProject.prototype.getSizeMap = function()
{
	return this.data.memStats.sizeMap;
}

/**********************************************************/
//--PORTED IN C++ VERSION--
MaltProject.prototype.getReallocMap = function()
{
	return this.data.memStats.reallocJump;
}

/**********************************************************/
/**
 * Extract a list of stacks containing elements which pass the given filter function.
 * @param filter A filter function which return a boolean and have prototype function(detailedStackEntry)
**/
//--PORTED IN C++ VERSION--
MaltProject.prototype.getFilterdStacks = function(filter)
{
	//get some refs
	var stats = this.data.stacks.stats;
	var res = new Array();	

	//loop on stat entries
	for(var i in stats)
	{
		//extract some short pointers
		var detailedStack = stats[i].detailedStack;
		var info = stats[i].infos;

		//check if conteain
		if (filterExtractStacksCandidate(detailedStack,filter))
			res.push({stack:detailedStack,info:info});
	}

	return res;
}

/**********************************************************/
/**
 * Return the list of stacks (detailed) which contain location file:line.
**/
//--PORTED IN C++ VERSION--
MaltProject.prototype.getFilterdStacksOnFileLine = function(file,line)
{
	return this.getFilterdStacks(function(entry) {
		return entry.file == file && entry.line == line;
	});
}

/**********************************************************/
/**
 * Return the list of stacks (detailed) which contain location file:line.
**/
MaltProject.prototype.getFilterdStacksOnSymbol = function(symbol)
{
	return this.getFilterdStacks(function(entry) {
		return entry.function == symbol;
	});
}

/**********************************************************/
/**
 * Return all timed values to build graphs.
**/
//--PORTED IN C++ VERSION--
MaltProject.prototype.getTimedValues = function()
{
	var tmp = new Object();
	/*tmp.segments     = this.data.timeline.segments;
	tmp.internalMem  = this.data.timeline.internalMem;
	tmp.virtualMem   = this.data.timeline.virtualMem;
	tmp.physicalMem  = this.data.timeline.physicalMem;
	tmp.requestedMem = this.data.timeline.requestedMem;*/
	tmp.ticksPerSecond = this.data.globals.ticksPerSecond;
	tmp.allocBandwidth = this.data.timeline.allocBandwidth;
	tmp.freeBandwidth = this.data.timeline.freeBandwidth;
	tmp.memoryBandwidth = this.data.timeline.memoryBandwidth;
	tmp.allocCnt = this.data.timeline.allocCnt;
	tmp.freeCnt = this.data.timeline.freeCnt;
	tmp.memoryTimeline = this.data.timeline.memoryTimeline;
	tmp.systemTimeline = this.data.timeline.systemTimeline;
	return tmp;
}

/**********************************************************/
//--PORTED IN C++ VERSION--
MaltProject.prototype.genSummaryWarnings = function(data)
{
	//vars
	var ret = {};
	var runtime = data.run.runtime / data.system.ticksPerSecond;

	//check too large recycling ratio
	if (data.summary.recyclingRatio > 10)
	{
		ret.recyclingRatio = ["Caution, you are heavily recycling your memory, it might hurt performance, check the allocation rate."];
		ret.totalAllocatedMemory = ["Caution, you are heavily recycling your memory, it might hurt performance, check the allocation rate."];
	}

	if (data.summary.allocCount / runtime > 100000)
		ret.allocCount = ["Caution, you are doing really large number of memory allocation, it might hurt performance."];
	if (data.summary.leakedMem > data.summary.peakRequestedMemory / 2)
		ret.leakedMem = ["Caution, half of your memory has leaked, it might not be an issue, but maybe you need to ensure the segments are used during the whole program life."]
	if (data.summary.globalVarMem > data.summary.peakRequestedMemory / 3 && data.summary.globalVarMem > 1024*1024)
		ret.globalVarMem = ["Caution, a large part of your memory is consummed by global variables, check if it is normal."];
	if (data.summary.tlsVarMem > data.summary.peakRequestedMemory / 3 && data.summary.tlsVarMem > 1024*1024)
		ret.tlsVarMem = ["Caution, a large part of your memory is consummed by TLS variables, check if it is normal."];
	if (data.summary.numGblVar > 500)
		ret.numGblVar = ["Caution, you get a realy big number of global variable, your code is likely to be buggy."];

	return ret;
}

/**********************************************************/
/**
 * Build a summary from the whole datas.
**/
//--PORTED IN C++ VERSION--
MaltProject.prototype.getSummaryV2 = function()
{
	var ret = {};

	//extract run info
	ret.run = this.data.run;

	//extract system info
	ret.system = {};
	ret.system.totalMemory = this.data.globals.totalMemory
	ret.system.ticksPerSecond = this.data.globals.ticksPerSecond;

	//summary
	ret.summary = {};
	ret.summary.peakPhysicalMemory = this.data.timeline.memoryTimeline.peak[2];//this.data.timeline.physicalMem.peakMemory;
	ret.summary.peakVirtualMemory = this.data.timeline.memoryTimeline.peak[1];//this.data.timeline.virtualMem.peakMemory;
	ret.summary.peakRequestedMemory = this.data.timeline.memoryTimeline.peak[0];//this.data.timeline.requestedMem.peakMemory;
	ret.summary.peakInternalMemory = this.data.timeline.memoryTimeline.peak[3];//this.data.timeline.internalMem.peakMemory;
	ret.summary.peakSegmentCount = this.data.timeline.memoryTimeline.peak[4];//this.data.timeline.segments.peakMemory;

	//rates
	var p = 0;
	for (var i in this.data.timeline.memoryBandwidth.values)
		if (this.data.timeline.memoryBandwidth.values[i][1] > p)
			p = this.data.timeline.memoryBandwidth.values[i][1];
	ret.summary.peakAllocRate = (p / this.data.timeline.memoryBandwidth.perPoints) * this.data.globals.ticksPerSecond;

	p = 0;
	for (var i in this.data.timeline.memoryBandwidth.values)
		if (this.data.timeline.memoryBandwidth.values[i][3] > p)
			p = this.data.timeline.memoryBandwidth.values[i][3];
	ret.summary.peakAllocCountRate = (p / this.data.timeline.memoryBandwidth.perPoints) * this.data.globals.ticksPerSecond;

	//search min/max/count size
	var min = -1;
	var max = -1;
	var count = 0;
	var stats = this.data.stacks.stats;
	var sum = 0;
	for(var i in stats)
	{
		var info = stats[i].infos;
		if ((info.alloc.min < min || min == -1) && info.alloc.min > 0)
			min = info.alloc.min;
		if (info.alloc.max > max || max == -1)
			max = info.alloc.max;
		count += info.alloc.count;
		sum += info.alloc.sum;
	}

	//gen
	ret.summary.totalAllocatedMemory = sum;
	ret.summary.recyclingRatio = sum / ret.summary.peakRequestedMemory;
	ret.summary.allocCount = count;
	ret.summary.minAllocSize = min;
	ret.summary.meanAllocSize = sum / count;
	ret.summary.maxAllocSize = max;

	//leaks
	var leakCount = 0;
	var leakMem = 0;
	var leaks = this.data.leaks;
	for (var i in leaks)
	{
		leakCount += leaks[i].count;
		leakMem += leaks[i].memory;
	}
	ret.summary.leakedMem = leakMem;
	ret.summary.leakedCount = leakCount;

	//stacks
	ret.summary.largestStack = this.getMaxStack().size;

	//global vars
	var tlsMem = 0;
	var gblMem = 0;
	var cntVars = 0;
	var gvars = this.data.memStats.globalVariables;
	for (var i in gvars)
	{
		for (var j in gvars[i])
		{
			cntVars++;
			if (gvars[i][j].tls)
				tlsMem += gvars[i][j].size;
			else
				gblMem += gvars[i][j].size;
		}
	}
	ret.summary.numGblVar = cntVars;
	ret.summary.globalVarMem = gblMem;
	ret.summary.tlsVarMem = tlsMem * (this.data.globals.maxThreadCount + 1);

	//summary warnings
	ret.summaryWarnings = this.genSummaryWarnings(ret);

	//domains
	ret.summaryDomains = {
		"counters": this.data.domains.counters,
		"mem": this.data.domains.mem,
	};

	//thread stats
	ret.threadStats = [];
	for (var i in this.data.threads)
		ret.threadStats.push(this.data.threads[i].stats);

	//return
	return ret;
}

/**********************************************************/
/**
 * Build a summary from the whole datas.
**/
//--PORTED IN C++ VERSION--
MaltProject.prototype.getSummary = function()
{
	var ret = {};

	//extract global stats
	ret.globalStats = {};
	ret.globalStats.physicalMem = this.data.timeline.memoryTimeline.peak[2];//this.data.timeline.physicalMem.peakMemory;
	ret.globalStats.virtualMem = this.data.timeline.memoryTimeline.peak[1];//this.data.timeline.virtualMem.peakMemory;
	ret.globalStats.requestedMem = this.data.timeline.memoryTimeline.peak[0];//this.data.timeline.requestedMem.peakMemory;
	ret.globalStats.internalMemory = this.data.timeline.memoryTimeline.peak[4];//this.data.timeline.internalMem.peakMemory;
	ret.globalStats.segments = this.data.timeline.memoryTimeline.peak[3];//this.data.timeline.segments.peakMemory;

	//search min/max/count size
	var min = -1;
	var max = -1;
	var count = 0;
	var stats = this.data.stacks.stats;
	var sum = 0;
	for(var i in stats)
	{
		var info = stats[i].infos;
		if ((info.alloc.min < min || min == -1) && info.alloc.min > 0)
			min = info.alloc.min;
		if (info.alloc.max > max || max == -1)
			max = info.alloc.max;
		count += info.alloc.count;
		sum += info.alloc.sum;
	}

	//extract
	ret.globalStats.minChunkSize = min;
	ret.globalStats.maxChunkSize = max;
	ret.globalStats.count = count;
	ret.globalStats.largestStack = this.getMaxStack().size;
	ret.globalStats.cumulAllocs = sum;

	return ret;
}

/**********************************************************/
//--PORTED IN C++ VERSION--
MaltProject.prototype.getStacksMem = function()
{
	//prepare array
	var res = new Array();

	//copy informations
	for (var i in this.data.threads)
		res.push(this.data.threads[i].stackMem);

	//ok return
	return {stacks:res,ticksPerSecond:this.data.globals.ticksPerSecond};
}

/**********************************************************/
/**
 * Get info about the largest stack
**/
//--PORTED IN C++ VERSION--
MaltProject.prototype.getMaxStack = function()
{
	//get first to start
	var res = this.data.threads[0].stackMem;

	//loop
	for (var i in this.data.threads)
	{
		var tmp = this.data.threads[i];
		if (tmp.stackMem.size > res.size)
		{
			res = tmp.stackMem;
			console.log(JSON.stringify(res));
		}
	}

	return res;
}

/**********************************************************/
/**
 * Flatten datas about the largest stack and return as json tree.
**/
//--PORTED IN C++ VERSION--
MaltProject.prototype.getFlattenMaxStackInfo = function(mapping,accept,stack)
{
	//init hash map to flat on addresses
	var ret = new Object();
	//var maxStack = this.data.maxStack;
	//var maxStack = this.getMaxStack();
	var maxStack = stack;

	//loop on all entries
	for (var i = 0 ; i < maxStack.stack.length ; i++)
	{
		//get some vars
		var addr = maxStack.stack[i];
		var mem = maxStack.mem[i] - maxStack.mem[i+1];
		var info = this.data.sites.instr[addr];
		var key = addr;
		if (info != undefined)
			key = mapping(info);
		else
			info = {function:addr};

		//check filter
		if (accept == true || accept(info))
		{
			//create or merge
			if (ret[key] == undefined)
			{
				ret[key] = {info:info,count:1,mem:mem};
			} else {
				ret[key].mem += mem;
				ret[key].count++;
			}
		}
	}

	//remove keys
	var finalRes = [];
	for (var i in ret)
		finalRes.push(ret[i]);

	//ok return
	return {details:finalRes,totalMem:maxStack.size};
}

/**********************************************************/
/**
 * Flatten datas about the largest stack and return as json tree.
**/
//--PORTED IN C++ VERSION--
MaltProject.prototype.getMaxStackInfoOnFunction = function(mapping,accept)
{
	return this.getFlattenMaxStackInfo(
		function(info) {return info.function;},
		true,
		this.getMaxStack()
	);
}

/**********************************************************/
/**
 * Return true if the given path correspond to a source file of
 * the current project.
**/
//--PORTED IN C++ VERSION--
MaltProject.prototype.isSourceFile = function(path)
{
	return (this.data.sourceFiles[path] == true)
}

/**********************************************************/
/**
 * Flatten datas about the largest stack and return as json tree.
**/
//--PORTED IN C++ VERSION--
MaltProject.prototype.getStackInfoOnFunction = function(id)
{
	return this.getFlattenMaxStackInfo(
		function(info) {return info.function;},
		true,
		this.data.threads[id].stackMem
	);
}

/**********************************************************/
//--PORTED IN C++ VERSION--
MaltProject.prototype.getFullTree = function()
{
	var tree = {};
	var data = this.data;

	for (var i in data.stacks.stats)
	{
		var cur = tree;
		var stack = data.stacks.stats[i].stack;
		var infos = data.stacks.stats[i].infos;
		for (var j in stack)
		{
			if (cur[stack[j]] == undefined)
			{
				cur[stack[j]] = {};
				//cur[stack[j]].site = data.sites.instr[stack[j]];
			}
			cur = cur[stack[j]];
		}

		if (cur.infos == undefined)
			cur.infos = clone(infos);
		else
			mergeStackInfoDatas(cur.infos,infos);
	}

	return tree;
}

MaltProject.prototype.getCallTree = function(nodeId, depth, height, minCost, func, metric, isRatio, callback) {
	// Response object
	var resp = {};

	// If tree object hasnt been created, create and cache it
	if(!this.calltreeCache) {
		// console.time("filteredStack");
		var filteredStack = this.getFilterdStacks(true);
		// console.timeEnd("filteredStack");
		this.calltreeCache = new CallTreeAdapter(filteredStack);
	}

	// If nodeId not provided, get node id by function name
	if(!nodeId) {
		var tmpnode = this.calltreeCache.getNodeByFunctionName(func);

		if(tmpnode == null) {
			callback({error: {nodeNotFoundError: "Node not found."}});
			return;
		}

		nodeId = tmpnode.id;
	}

	// Filter tree and get the focal node
	// console.time("filterNodeLine");
	var filteredTree = null;
	var ratio = isRatio === 'true' ? true : false;
	if(nodeId == -1) {
		filteredTree = this.calltreeCache.filterRootLines(depth, minCost, metric, ratio);
	} else {
		filteredTree = this.calltreeCache.filterNodeLine(nodeId, depth, height, minCost, metric, ratio);
	}
	// console.timeEnd("filterNodeLine");

	// console.time("getNodeById");
	var node = this.calltreeCache.getNodeById(nodeId);
	// console.timeEnd("getNodeById");

	// Build output object
	resp.totalNodes = this.calltreeCache.getNodes().length;
	resp.visibleNodes = filteredTree.nodes.length;
	if(nodeId == -1) {
		resp.nodeId = -1;
		resp.file = "Root nodes";
		resp.fileShort = resp.file;
		resp.function = "Filtering might hide some nodes";
		resp.functionShort = resp.function;
	} else {
		resp.nodeId = nodeId;
		resp.file = node.data.location.file;
		resp.fileShort = (resp.file.length > 40) ? '.../' + resp.file.replace(/^.*[\\\/]/, '') : resp.file;
		resp.function = node.data.location.function;
		resp.functionShort = (resp.function.length > 40) ? node.label : resp.function;
	}
	// console.time("getDotCodeForTree");
	resp.dotCode = GraphGenerator.getDotCodeForTree(filteredTree, nodeId);
	// console.timeEnd("getDotCodeForTree");
	resp.svg = null;

	// Generate SVG code from Dot code if GraphViz is installed
	if(GraphGenerator.isInstalled()) {
		// console.time("convertDotToSvg");
		if(filteredTree.nodes.length > 1000) {
			resp.error = {filterError: "Too many nodes selected at once."};
			callback(resp);
		} else {
			GraphGenerator.convertDotToSvg(resp.dotCode, function(svg, err) {
				// console.timeEnd("convertDotToSvg");
				if(err) {
					resp.error = {svgGenerationError: "Could not generate graph."};
				} else {
					resp.svg = svg;
				}
				callback(resp);
			});
		}
	} else {
		resp.error = {svgGenerationError: "Please install GraphViz to enable graph generation."};
		callback(resp);
	}
}

MaltProject.prototype.getActiveChunks = function(timestamp, callback) {
	// Always check if timestamp is a valid number to prevent RCE attacks on server
	if(!(!isNaN(timestamp) && (function(x) { return (x | 0) === x; })(parseFloat(timestamp)))) {
		callback({
			error: "Timestamp value is not a valid integer."
		});
		return;
	}

	var traceFileName = this.getTraceFilename();
	if(!traceFileName) {
		callback({
			error: "Trace file was not generated during profile time."
		});
		return;
	}

	var data = this.data.stacksById;

	var trace = childProcess.execFile('malt-trace-reader', ['--stats', '--filter', 'at=' + timestamp, traceFileName], function(err, stdout, stderr) {
		if(err) {
			callback({
				error: "Could not get trace data from trace file."
			});
			return;
		}

		var rawTraceData = null;
		try {
			rawTraceData = JSON.parse(stdout);
		} catch (e) {
			callback({
				error: "Could not parse data got from the trace file."
			});
			return;
		}

		var traceData = [];
		for (var i = 0; i < rawTraceData.length; i++) {
			traceData.push({
				stackId: rawTraceData[i][0],
				chunkCount: rawTraceData[i][1],
				chunkTotalSize: rawTraceData[i][2],
				chunkMinSize: rawTraceData[i][3],
				chunkMaxSize: rawTraceData[i][4],
				stack: data[rawTraceData[i][0]],
			});
		}

		callback({
			timestamp: timestamp,
			resultCount: traceData.length,
			result: traceData
		});
	});
}

/**********************************************************/
MaltProject.prototype.toCallgrindFormat = function()
{
	//obj to store infos pre-sorted for output
	var res = {

	};

	//loop on each stacks
	console.log(this);
	for (var i in this.data.stacks.stats)
	{
		var stack = this.data.stacks.stats[i];
		var callerCallee = this.getStackCallerCalle(stack);

	}
	return res;
}

/**********************************************************/
MaltProject.prototype.getStackCallerCalle = function(stack)
{
	console.log(stack);
	//leafCallee =
}

/**********************************************************/
MaltProject.prototype.stackIsMatchingBellowDepth = function(stack1, stack2, depth)
{
	//trivial
	if (depth == 0)
		return true;
	if (stack1.length < depth || stack2.length < depth)
		return false;

	//loop
	for (var i = 0 ; i < depth ; i++)
		if (!(stack1[stack1.length - i - 1].function == stack2[stack2.length - i - 1].function))
			return false;

	//ok
	return true;
}

/**********************************************************/
MaltProject.prototype.stackIsMatchingLocationFilter = function(filter, detailedStack)
{
	//nothing to chec
	if (filter.function == "" && filter.file == "" && filter.line == -1)
		return true;

	for (var i = 0 ; i < detailedStack.length ; i++) {
		var location = detailedStack[i];

		//check
		if (filter.function != "" && filter.function == location.function)
			return true;
		if (filter.file != "" && filter.file == location.file && filter.line != -1 && filter.line == location.line)
			return true;
	}

	//not ok
	return false;
}

/**********************************************************/
MaltProject.prototype.getCallStackNextLevel = function(parentStackId, parentDepth, filter)
{
	//vars
	var instrs = this.data.sites.instr;
	var parentStack = this.data.stacks.stats[parentStackId].stack;
	var detailedParentStack = genDetailedStack(instrs,parentStack);
	var result = [];
	var alreadySeen = {};

	//search stacks starting by
	for (var i = 0 ; i < this.data.stacks.stats.length ; i++) {
		//get ref
		it = this.data.stacks.stats[i];
		var detailedStack = genDetailedStack(instrs,it.stack);

		//check ok
		var stackMatchingBellowDepth = this.stackIsMatchingBellowDepth(detailedParentStack, detailedStack, parentDepth);
		var stackMatchingFilter = this.stackIsMatchingLocationFilter(filter, detailedStack);
		if (stackMatchingBellowDepth && stackMatchingFilter && it.stack.length > parentDepth) {
			//sum all childs up to here
			var hasChild = false;
			if (it.stack.length > parentDepth + 1)
				hasChild = true;

			//get next child
			var location = detailedStack[it.stack.length - parentDepth - 1];
			bufferRef = location.file + ":" + location.function;

			//check already seen
			var it2 = alreadySeen[bufferRef];
			if (it2 == undefined) {
				alreadySeen[bufferRef] = {
					infos: clone(it.infos),
					location : location,
					parentStackId: parentStackId,
					parentStackDepth : parentDepth,
					stackId: i,
					stackDepth: parentDepth + 1,
					hasChild: hasChild
				};
			} else {
				mergeStackInfoDatas(it2.infos, it.infos);
				it2.hasChild |= hasChild;
			}
		}
	}

	//convert
	for (var key in alreadySeen){
		result.push(alreadySeen[key]);
	}

	//ok
	return result;
}

/**********************************************************/
function filterExtractStacksCandidate(detailedStack,filter)
{
	for (var i in detailedStack)
		if (filter == true || filter(detailedStack[i]))
			return true;
	return false;
}

/**********************************************************/
/** Regexp to detect memory functions (new, new[], gnu and icc fortran alloc/free...). **/
//--PORTED IN C++ VERSION--
var allocFuncRegexp = /^((MALT::WrapperPython.*::.*)|(gomp_realloc)|(gomp_malloc)|(gomp_free)|(__gnu_cxx::new_allocator)|(operator new)|(operator delete)|(_Zn[wa])|(g_malloc)|(g_realloc)|(g_free)|(for__get_vm)|(for__free_vm)|([mc]alloc)|(free)|(realloc)|(memalign)|(posix_memalign)|(for_(de)?alloc_allocatable)|(for_(de)?allocate)|(default_malloc)|(default_realloc)|(default_calloc)|(default_free))/

/**********************************************************/
/** Quick check to detect memory functions. **/
//--PORTED IN C++ VERSION--
function isAllocFunction(data, name)
{
	if (data.run.allocatorWrappers.includes(name))
		return true;
	else
		return allocFuncRegexp.test(name);
}

/**********************************************************/
//--PORTED IN C++ VERSION--
function mergeStackMinMaxInfo(onto,value)
{
	onto.count += value.count;
	onto.sum += value.sum;
	if (onto.min == 0 || (value.min < onto.min && value.min != 0))
		onto.min = value.min;
	if (onto.max == 0 || (value.max > onto.max && value.max != 0))
		onto.max = value.max;
}

/**********************************************************/
//--PORTED IN C++ VERSION--
function mergeStackInfoDatas(onto,value)
{
	onto.countZeros += value.countZeros;
	onto.maxAliveReq += value.maxAliveReq;
	onto.aliveReq += value.aliveReq;
	onto.globalPeak += value.globalPeak;
	onto.reallocCount += value.reallocCount;
	onto.reallocSumDelta += value.reallocSumDelta;
	mergeStackMinMaxInfo(onto.alloc,value.alloc);
	mergeStackMinMaxInfo(onto.free,value.free);
	mergeStackMinMaxInfo(onto.lifetime,value.lifetime);
}

/**********************************************************/
//--PORTED IN C++ VERSION--
function mergeStackInfo(into,detailedStackEntry,addr,subKey,infos,mapping,fields)
{
	//extract key by using mapping function
	var key = mapping(detailedStackEntry,infos);
	if (key == undefined)
		key = addr;

	//check existing
	var cur = into[key];
	if (cur == undefined)
	{
		//not exist -> create
		cur = new Object();

		//copy user requested fields
		for (var i in fields)
			cur[fields[i]] = detailedStackEntry[fields[i]];

		//add
		into[key] = cur;
	} else {
		//check line and keep the lowest one
		if ((detailedStackEntry.line > 0 && detailedStackEntry.line < cur.line) || cur.line <= 0 )
			cur.line = detailedStackEntry.line;
	}

	//check for subkey (own or total) and clone or merge
	if (cur[subKey] == undefined)
		cur[subKey] = clone(infos);
	else
		mergeStackInfoDatas(cur[subKey],infos);
}

/**********************************************************/
/**
 * Short wrapper to get strings from data.stacks.strings section and to manage undefined files.
 * @param strings Must be the translation table from sites.strings
**/
//--PORTED IN C++ VERSION--
function getStringFromList(strings,id,defaultValue)
{
	if (id == undefined || id == -1)
	{
		return defaultValue;
	} else {
		//if (strings[id] == '')
		//	console.log("Get '' string "+id+"->"+strings.length);
		var res = strings[id];
		if (res == undefined)
			return defaultValue;
		else
			return res;
	}
}

/**********************************************************/
/**
 * To help quick search into data, procude a stack with refs to internal details instead of
 * simple addresses.
 * @param instrs Must be the translation table from sites.instr
**/
function genDetailedStack(instrs,stack)
{
	//create final array
	var res = new Array();

	//loop on entries and match
	for (var i in stack)
	{
		//search detaisl
		var details = instrs[stack[i]];
		if (details == undefined)
			throw Error("Cannot find details for instruction " + stack[i]);

		//push into detailed stack
		res.push(details);
	}

	//ok return
	return res;
}

/**********************************************************/
var gccFortModRegex = /__([A-Za-z0-9_]+)_MOD_([A-Za-z0-9_]+)/;
function cleanupFunctionName(funcName)
{
	//fortran GCC mods
	var res = gccFortModRegex.exec(funcName);

	//console.log("Detect fortran module : "+res[1]+"::"+res[2]);

	if (res != null)
		return res[1]+"::"+res[2];
	else
		return funcName;
}

/**********************************************************/
function rebuildChilds(finalStacks,stack,cur,data,addresses)
{
	//push data if has some
	if (cur.dataId != undefined)
	{
		//extract infos
		var globals = data.globals[cur.dataId];
		var realloc = data.realloc[cur.dataId];
		var alloc = data.alloc[cur.dataId];
		var free = data.free[cur.dataId];
		var lifetime = data.lifetime[cur.dataId];

		//fill
		finalStacks.push({
			stack: clone(stack),
			stackId: 'TODO',
			infos: {
				countZeros: globals.zero,
				maxAliveReq: globals.alive,
				aliveReq: globals.alive,
				globalPeak: globals.peak,
				reallocCount: (realloc != undefined) ? realloc.count : 0,
				reallocSumDelta: (realloc != undefined) ? realloc.delta : 0,
				alloc: {
					count: (alloc != undefined) ? alloc.count : 0,
					min: (alloc != undefined) ? alloc.min : 0,
					max: (alloc != undefined) ? alloc.max : 0,
					sum: (alloc != undefined) ? alloc.sum : 0,
				},
				free: {
					count: (free != undefined) ? free.count : 0,
					min: (free != undefined) ? free.min : 0,
					max: (free != undefined) ? free.max : 0,
					sum: (free != undefined) ? free.sum : 0,
				},
				lifetime: {
					count: (lifetime != undefined) ? lifetime.count : 0,
					min: (lifetime != undefined) ? lifetime.min : 0,
					max: (lifetime != undefined) ? lifetime.max : 0,
					sum: (lifetime != undefined) ? lifetime.sum : 0,
				},
			}
		})
	}

	//childs
	for (var i in cur)
	{
		if (i != "dataId")
		{
			//convert childs
			stack.push(addresses[i]);
			rebuildChilds(finalStacks,stack,cur[i],data,addresses);
			stack.pop();
			//free old memory
			cur[i] = undefined
		}
	}
}

/**********************************************************/
/**
 * Reshape the stack structure to fit with the original version, ideally
 * need to move all the gui code the the new one which seams better.
 * But this way to convert seams faster as I don't have time (TODO).
**/
function rebuildStacks(data)
{
	//don't need
	if (data.stacks.stats != undefined)
		return;

	//message
	console.log("Start to convert tree representation to old flat for compat")

	//run
	var finalStacks = [];
	for (var i in data.stacks.calltree)
	{
		var stack = [];
		stack.push(data.stacks.addresses[i]);
		rebuildChilds(finalStacks,stack,data.stacks.calltree[i],data.stacks.data,data.stacks.addresses);
		stack.pop();
	}
	data.stacks.stats = finalStacks;
	//console.log(finalStacks);
}

/**********************************************************/
/**
 * Reorganize a little but the datas to get quicker access on requests. Mosty re-established the
 * in memory references between call site addresses and their textual definitions (line, file...).
**/
function optimizeProjectDatas(data)
{
	//rebuild stack
	rebuildStacks(data);

	//get some inside vars
	var strings = data.sites.strings;
	var instrs = data.sites.instr;

	//do for stacks/instr section
	//avoid to jump to string table every time
	console.log("Optimizing sites.instr...");
	data.sourceFiles = {};
	for (var i in data.sites.instr)
	{
		var site = data.sites.instr[i];
		//var old = site.function;
		site.function = getStringFromList(strings,site.function,"??");
		site.function = cleanupFunctionName(site.function);
		//if (site.function == '')
		//	console.log("get '' => "+old);
		site.file = getStringFromList(strings,site.file,"??");
		if (site.file != '??')
			data.sourceFiles[site.file] = true;
		if (site.line == undefined)
			site.line = -1;
	}

	//add detailedStack field to entries from stckInfo.stats and leaks
	//avoid to jump to instr table every time
	console.log("Optimizing access to stack details...");
	for (var i in data.stacks.stats)
	{
		var cur = data.stacks.stats[i];
		cur.detailedStack = genDetailedStack(instrs,cur.stack);
	}

	var stackIdMap = {};
	for (var i in data.stacks.stats)
	{
		stackIdMap[data.stacks.stats[i].stackId] = data.stacks.stats[i].detailedStack;
	}
	data.stacksById = stackIdMap;

	//same for leaks
	for (var i in data.leaks)
	{
		var cur = data.leaks[i];
		cur.detailedStack = genDetailedStack(instrs,cur.stack);
	}
}

/**********************************************************/
//export definition
module.exports = MaltProject;
