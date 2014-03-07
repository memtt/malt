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
	this.loadFile(file);
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
		
		//optimize data
		console.log("Optimizing datas for requests...");
		optimizeProjectDatas(data);
		console.log("Data optimization done.");
		
		//ok done
		this.data = data;
	});
}

/****************************************************/
/**
 * Produce a flat profile by projecting stats onto sumbols. You can get some simple examples by going to getFileLinesFlatProfile() or getFunctionFlatProfile()
 * @param mapping Provide a function whith prototype function(entry,info) which return one of the entry field 
 * to be used as fusion criteria. Entry correspond to entries from stackInfo.stats
 * @param accept Can be 'true' or a function with prototype(entry,info) with entry from stackInfo.stats[].detailedStack to accept (true) or reject (false) them.
 * @param total If 'true', the output contain 'own' and 'total' otherwise it contain 'own' and 'childs'.
**/
MattProject.prototype.getFlatProfile = function(mapping,accept,fields,total)
{
	//setup some local vars
	var stats = data.stackInfo.stats;
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
		['line'],                                    //export only line info
		total);
	return res;
}

/****************************************************/
/** Regexp to detect memory functions. **/
var allocFuncRegexp = /^((_Zn[wa])|(g_malloc)|(g_realloc)|(g_free)|(for__get_vm)|(for__free_vm))/

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
	mergeStackMinMaxInfo(onto.alloc,value.alloc);
	mergeStackMinMaxInfo(onto.free,value.free);
	mergeStackMinMaxInfo(onto.lifetime,value.lifetime);
}

/****************************************************/
function mergeStackInfo(into,detailedStackEntry,addr,subKey,value,mapping,fields)
{
	var key = mapping(detailedStackEntry);
	if (key == undefined)
		key = addr;

	var cur = into[key];
	if (cur == undefined)
	{
		cur = new Object();
		//cur = clone(detailedStackEntry);
		for (var i in fields)
			cur[fields[i]] = detailedStackEntry[fields[i]];
		into[key] = cur;
	} else {
		if (detailedStackEntry.line != 0 && detailedStackEntry.line != -1 && (detailedStackEntry.line < cur.line || cur.line == -1 || cur.line == 0))
			cur.line = detailedStackEntry.line;
	}
	
	if (cur[subKey] == undefined)
		cur[subKey] = clone(value);
	else
		mergeStackInfoDatas(cur[subKey],value);
}

/****************************************************/
/**
 * Short wrapper to get strings from data.stackInfo.strings section and to manage undefined files.
 * @param strings Must be the translation table from stackInfo.sites.strings
**/
function getStringFromList(strings,id,defaultValue)
{
	if (id == undefined || id == -1)
	{
		return defaultValue;
	} else {
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
 * @param instrs Must be the translation table from stackInfo.sites.instr
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
/**
 * Reorganize a little but the datas to get quicker access on requests. Mosty re-established the
 * in memory references between call site addresses and their textual definitions (line, file...).
**/
function optimizeProjectDatas(data)
{
	//get some inside vars
	var strings = data.stackInfo.sites.strings;
	var instrs = data.stackInfo.sites.instr;
	
	//do for stackInfo/instr section
	console.log("Optimizing stackInfo.sites.instr...");
	for (var i in data.stackInfo.sites.instr)
	{
		var site = data.stackInfo.sites.instr[i];
		site.function = getStringFromList(strings,site.function,"??");
		site.file = getStringFromList(strings,site.file,"??");
		if (site.line == undefined)
			site.line = -1;
	}
	
	//add detailedStack field to entries from stckInfo.stats and leaks
	console.log("Optimizing access to stack details...");
	for (var i in data.stackInfo.stats)
	{
		var cur = data.stackInfo.stats[i];
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
