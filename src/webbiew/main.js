/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/****************************************************/
//Deps
var fs      = require('fs');
var Args    = require('arg-parser');
var Express = require('express');
var clone   = require('clone');

/****************************************************/
//start express
var app = Express();

/****************************************************/
//Manage args
args = new Args('matt-webview', '1.0', 'Webiew for MATT based on Node.js','');
//define args
args.add({ name: 'input', desc: 'input file from MATT into JSON format', switches: [ '-i', '--input-file'], value: 'file', required: true });
if (!args.parse()) 
{
	console.error("Invalid parameters, please check with -h");
	process.exit(1);
}

/****************************************************/
//load file
var data = null;
console.log("Loading file : " + args.params.input);
fs.readFile(args.params.input, 'utf8', function (err, buffer) {
	if (err) {
		console.log('Error: ' + err);
		return;
	}
	data = JSON.parse(buffer);
});

/****************************************************/
function MattGetFunction(addr)
{
	var site = data.stackInfo.sites.instr[addr];
	
	//not found
	if (site == undefined || site == -1)
		return addr;

	//extract func ID
	var func = data.stackInfo.sites.strings[site.function];
	
	//not found
	if (func == undefined)
		return addr;

	//ok
	return func;
}

/****************************************************/
function MattGetString(value,resIfNotFound)
{
	if (value == undefined || value == -1)
		return resIfNotFound;

	//extract func ID
	var res = data.stackInfo.sites.strings[value];
	
	//not found
	if (res == undefined)
		return resIfNotFound;

	//ok
	return res;
}

/****************************************************/
function MattMergeMinMax(onto,value)
{
	onto.count += value.count;
	onto.min += value.min;
	onto.max += value.max;
	onto.sum += value.sum;
}

/****************************************************/
function MattReduceStackInfo(onto,value)
{
	onto.countZeros += value.countZeros;
	onto.maxAliveReq += value.maxAliveReq;
	onto.aliveReq += value.aliveReq;
	MattMergeMinMax(onto.alloc,value.alloc);
	MattMergeMinMax(onto.free,value.free);
	MattMergeMinMax(onto.lifetime,value.lifetime);
}

/****************************************************/
function MattReduceStackInfoObject(into,addr,subKey,value)
{
	var site = data.stackInfo.sites.instr[addr];
	var key = addr;
	if (site != undefined)
		key = MattGetString(site.function,addr);

	var cur = into[key];
	if (cur == undefined)
	{
		into[key] = new Object();
		cur = into[key];
		cur.file = MattGetString(site.file,"??");
		cur.line = site.line;
	}
	
	if (into[key][subKey] == undefined)
		into[key][subKey] = clone(value);
	else
		MattReduceStackInfo(into[key][subKey],value);
}

/****************************************************/
//map on datas/flat.json to provide flat profiles on functions
app.get('/flat.json',function(req,res) {
	console.log('Get flat profile /flat.json');

	//loop on all entries and compute reduced flat view
	var tmp = new Object;
	var stats = data.stackInfo.stats;
	for(var i in stats)
	{
		//extract some short pointers
		var stack = stats[i].stack;
		var infos = stats[i].infos;

		//update internal values
		MattReduceStackInfoObject(tmp,stack[0],"internal",infos);
		
		//childs
		var done = new Object;
		for (var j in stack)
		{
			var func = MattGetFunction(stack[j]);
			if (done[func] == undefined)
			{
				done[func] = true;
				MattReduceStackInfoObject(tmp,stack[j],"childs",infos);
			}
		}
	}

	//ok flush to user
	res.write(JSON.stringify({'funcs':tmp},null,"\t"));
	res.end();
});

/****************************************************/
//export static deps
app.use('/deps/jquery',Express.static(__dirname + '/client_deps/jquery-1.11.0'));
app.use('/deps/bootstrap',Express.static(__dirname + '/client_deps/bootstrap-3.1.1-dist'));
app.use('/deps/bootswatch',Express.static(__dirname + '/client_deps/bootswatch.com/slate'));
app.use('/deps/ejs',Express.static(__dirname + '/client_deps/ejs-1.0/'));
app.use('/',Express.static(__dirname+'/client_files'));

/****************************************************/
//run express
console.log("Starting server on http://localhost:8080");
app.listen(8080);
