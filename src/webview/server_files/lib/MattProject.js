/****************************************************/
//deps
var fs    = require('fs');
var clone = require('clone');

/****************************************************/
/**
 * Construct a MattProject by loading data in JSON format from given file.
**/
function MattProject(file)
{
	//declare internal stats to get a short list in same place
	this.data = null;//store data tree
	this.file = null;//more for debug to remember the related data file
	
	//load file
	if (file != undefined)
		this.loadFile(file);
}

/****************************************************/
MattProject.prototype.loadData = function(data)
{
	//setup current data
	this.data = data;
	
// 	this.data.stacks = this.getFullTree();
// 	console.log(JSON.stringify(data));
// 	this.data = null;

	//optimize data
	console.log("Optimizing datas for requests...");
	optimizeProjectDatas(data);
	console.log("Data optimization done.");
}

/****************************************************/
/**
 * Function in charge of loading the json file.
**/
MattProject.prototype.loadFile = function(file)
{
	//init
	this.data = null;
	this.file = file;
	var cur = this;
	console.log("Loading file : " + args.params.input);
	
	//read from FS
	fs.readFile(args.params.input, 'utf8', function (err, buffer) {
		//manage errors
		if (err) {
			var msg = 'Error: ' + err;
			console.log(msg);
			throw new Error(msg);
		}
		
		//ok parse
		var data = JSON.parse(buffer);
		
		cur.loadData(data);
	});
}

/****************************************************/
/**
 * Just for debug, print only stack with function names.
**/
MattProject.prototype.getDebugStackList = function()
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

/****************************************************/
/**
 * Produce a flat profile by projecting stats onto sumbols. You can get some simple examples by going to getFileLinesFlatProfile() or getFunctionFlatProfile()
 * @param mapping Provide a function whith prototype function(entry,info) which return one of the entry field 
 * to be used as fusion criteria. Entry correspond to entries from stacks.stats
 * @param accept Can be 'true' or a function with prototype(entry,info) with entry from stacks.stats[].detailedStack to accept (true) or reject (false) them.
 * @param total If 'true', the output contain 'own' and 'total' otherwise it contain 'own' and 'childs'.
**/
MattProject.prototype.getFlatProfile = function(mapping,accept,fields,total)
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
		while (isAllocFunction(detailedStack[skip].function)) skip++;
		
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
			if (filter && done[key] == undefined && !isAllocFunction(cur.function))
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

/****************************************************/
/**
 * Map memory informations from stack onto file lines.
 * @param total If 'true', produce 'own' and 'total', otherwise produce 'own' and 'childs'.
**/
MattProject.prototype.getFileLinesFlatProfile = function(file,total)
{
	var res = this.getFlatProfile(
		function(entry) {return entry.line;},        //map on lines
		function(entry) {return entry.file == file}, //accept only requested file
		['line','function'],                         //export only line info
		total);
	return res;
}

/****************************************************/
/**
 * Map memory informations from stack on functions (symbols).
 * @param total If 'true', produce 'own' and 'total', otherwise produce 'own' and 'childs'.
**/
MattProject.prototype.getFlatFunctionProfile = function(total)
{
	var res = this.getFlatProfile(
		function(entry) {return entry.function;},    //map on lines
		true,                                        //accept all
		['function','line','file'],                  //export only line info
		total);
	return res;
}

/****************************************************/
/**
 * Return virtual memory distribution extracted from /proc/self/maps and execution end.
**/
MattProject.prototype.getProcMapDistr = function()
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

/****************************************************/
MattProject.prototype.getSizeMap = function()
{
	return this.data.memStats.sizeMap;
}

/****************************************************/
/**
 * Extract a list of stacks containing elements which pass the given filter function.
 * @param filter A filter function which return a boolean and have prototype function(detailedStackEntry)
**/
MattProject.prototype.getFilterdStacks = function(filter)
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

/****************************************************/
/**
 * Return the list of stacks (detailed) which contain location file:line.
**/
MattProject.prototype.getFilterdStacksOnFileLine = function(file,line)
{
	return this.getFilterdStacks(function(entry) {
		return entry.file == file && entry.line == line;
	});
}

/****************************************************/
/**
 * Return the list of stacks (detailed) which contain location file:line.
**/
MattProject.prototype.getFilterdStacksOnSymbol = function(symbol)
{
	return this.getFilterdStacks(function(entry) {
		return entry.function == symbol;
	});
}

/****************************************************/
/**
 * Return all timed values to build graphs.
**/
MattProject.prototype.getTimedValues = function()
{
	var tmp = new Object();
	tmp.segments     = this.data.timeline.segments;
	tmp.internalMem  = this.data.timeline.internalMem;
	tmp.virtualMem   = this.data.timeline.virtualMem;
	tmp.physicalMem  = this.data.timeline.physicalMem;
	tmp.requestedMem = this.data.timeline.requestedMem;
	tmp.ticksPerSecond = this.data.globals.ticksPerSecond;
	tmp.allocBandwidth = this.data.timeline.allocBandwidth;
	tmp.freeBandwidth = this.data.timeline.freeBandwidth;
	return tmp;
}

/****************************************************/
/**
 * Build a summary from the whole datas.
**/
MattProject.prototype.getSummary = function()
{
	var ret = {};

	//extract global stats
	ret.globalStats = {};
	ret.globalStats.segments = this.data.timeline.segments.peakMemory;
	ret.globalStats.internalMemory  = this.data.timeline.internalMem.peakMemory;
	ret.globalStats.virtualMem  = this.data.timeline.virtualMem.peakMemory;
	ret.globalStats.requestedMem  = this.data.timeline.requestedMem.peakMemory;
	ret.globalStats.physicalMem  = this.data.timeline.physicalMem.peakMemory;
	
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

/****************************************************/
MattProject.prototype.getStacksMem = function()
{
	//prepare array
	var res = new Array();
	
	//copy informations
	for (var i in this.data.threads)
		res.push(this.data.threads[i].stackMem);
	
	//ok return
	return {stacks:res,ticksPerSecond:this.data.globals.ticksPerSecond};
}

/****************************************************/
/**
 * Get info about the largest stack
**/
MattProject.prototype.getMaxStack = function()
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

/****************************************************/
/**
 * Flatten datas about the largest stack and return as json tree.
**/
MattProject.prototype.getFlattenMaxStackInfo = function(mapping,accept,stack)
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

/****************************************************/
/**
 * Flatten datas about the largest stack and return as json tree.
**/
MattProject.prototype.getMaxStackInfoOnFunction = function(mapping,accept)
{
	return this.getFlattenMaxStackInfo(
		function(info) {return info.function;},
		true,
		this.getMaxStack()
	);
}

/****************************************************/
/**
 * Return true if the given path correspond to a source file of
 * the current project.
**/
MattProject.prototype.isSourceFile = function(path)
{
	return (this.data.sourceFiles[path] == true)
}

/****************************************************/
/**
 * Flatten datas about the largest stack and return as json tree.
**/
MattProject.prototype.getStackInfoOnFunction = function(id)
{
	return this.getFlattenMaxStackInfo(
		function(info) {return info.function;},
		true,
		this.data.threads[id].stackMem
	);
}

/****************************************************/
MattProject.prototype.getFullTree = function()
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

/****************************************************/
function filterExtractStacksCandidate(detailedStack,filter)
{
	for (var i in detailedStack)
		if (filter == true || filter(detailedStack[i]))
			return true;
	return false;
}

/****************************************************/
/** Regexp to detect memory functions (new, new[], gnu and icc fortran alloc/free...). **/
var allocFuncRegexp = /^((_Zn[wa])|(g_malloc)|(g_realloc)|(g_free)|(for__get_vm)|(for__free_vm)|([mc]alloc)|(free)|(realloc)|(memalign)|(posix_memalign)|(for_(de)?alloc_allocatable)|(for_(de)?allocate))/

/****************************************************/
/** Quick check to detect memory functions. **/
function isAllocFunction(name)
{
	return allocFuncRegexp.test(name);
}

/****************************************************/
function mergeStackMinMaxInfo(onto,value)
{
	onto.count += value.count;
	onto.sum += value.sum;
	if (onto.min == 0 || (value.min < onto.min && value.min != 0))
		onto.min = value.min;
	if (onto.max == 0 || (value.max > onto.max && value.max != 0))
		onto.max = value.max;
}

/****************************************************/
function mergeStackInfoDatas(onto,value)
{
	onto.countZeros += value.countZeros;
	onto.maxAliveReq += value.maxAliveReq;
	onto.aliveReq += value.aliveReq;
	onto.globalPeak += value.globalPeak;
	mergeStackMinMaxInfo(onto.alloc,value.alloc);
	mergeStackMinMaxInfo(onto.free,value.free);
	mergeStackMinMaxInfo(onto.lifetime,value.lifetime);
}

/****************************************************/
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
		if (detailedStackEntry.line != 0 && detailedStackEntry.line != -1 && (detailedStackEntry.line < cur.line || cur.line == -1 || cur.line == 0))
			cur.line = detailedStackEntry.line;
	}
	
	//check for subkey (own or total) and clone or merge
	if (cur[subKey] == undefined)
		cur[subKey] = clone(infos);
	else
		mergeStackInfoDatas(cur[subKey],infos);
}

/****************************************************/
/**
 * Short wrapper to get strings from data.stacks.strings section and to manage undefined files.
 * @param strings Must be the translation table from sites.strings
**/
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

/****************************************************/
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

/****************************************************/
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

/****************************************************/
/**
 * Reorganize a little but the datas to get quicker access on requests. Mosty re-established the
 * in memory references between call site addresses and their textual definitions (line, file...).
**/
function optimizeProjectDatas(data)
{
	//get some inside vars
	var strings = data.sites.strings;
	var instrs = data.sites.instr;
	
	//TODO remove
	for (var i in strings)
		if (strings[i] == '')
			console.log("???????????? => "+i+" -> "+strings.length);
	
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

	//same for leaks
	for (var i in data.leaks)
	{
		var cur = data.leaks[i];
		cur.detailedStack = genDetailedStack(instrs,cur.stack);
	}
}

/****************************************************/
//export definition
module.exports = MattProject;
