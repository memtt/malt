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
var engine  = require('ejs-locals');

/****************************************************/
//start express
var app = Express();
app.engine('ejs',engine);
app.set('views',__dirname + '/server_files/views');
app.set('view engine','ejs');

/****************************************************/
//intenral cache for computed data which take a while to built
var mattCache = new Object();

/****************************************************/
//Manage args
args = new Args('matt-webview', '1.0', 'Webiew for MATT based on Node.js','');
//define args
args.add({ name: 'input', desc: 'input file from MATT into JSON format', switches: [ '-i', '--input-file'], value: 'file', required: true });
args.add({ name: 'port',  desc: 'Port to use to wait for HTTP requests', switches: [ '-p', '--port'],       value: 'port', required: false });
if (!args.parse()) 
{
	console.error("Invalid parameters, please check with -h");
	process.exit(1);
}

/****************************************************/
var port = 8080;
if (args.params.port != undefined)
	port = args.params.port;

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
	onto.sum += value.sum;
	if (onto.min == 0 || (value.min < onto.min && value.min != 0))
		onto.min = value.min;
	if (onto.max == 0 || (value.max > onto.max && value.max != 0))
		onto.max = value.max;
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
	if (site == undefined)
	{
		site = new Object();
		site.file = '??';
		site.line = -1;
	}

	var cur = into[key];
	if (cur == undefined)
	{
		into[key] = new Object();
		cur = into[key];
		cur.file = MattGetString(site.file,"??");
		cur.line = site.line;
	} else {
		if (site.line != 0 && site.line != -1 && (site.line < cur.line || cur.line == -1 || cur.line == 0))
			cur.line = site.line;
	}
	
	if (into[key][subKey] == undefined)
		into[key][subKey] = clone(value);
	else
		MattReduceStackInfo(into[key][subKey],value);
}

/****************************************************/
function MattIsCPPOperator(name)
{
	var begin = name.substring(0, 4);
	return (begin == "_Znw" || begin == "_Zna");
}

/****************************************************/
//map on datas/flat.json to provide flat profiles on functions
app.get('/flat.json',function(req,res) {
	var tmp;
	if (mattCache['flat.json'] != undefined)
	{
		tmp = mattCache['flat.json'];
	} else {
		//loop on all entries and compute reduced flat view
		var tmp = new Object;
		var stats = data.stackInfo.stats;
		for(var i in stats)
		{
			//extract some short pointers
			var stack = stats[i].stack;
			var infos = stats[i].infos;
			
			//skip C++ operators
			var skip = 0;
			while (MattIsCPPOperator(MattGetFunction(stack[skip]))) skip++;

			//update internal values
			MattReduceStackInfoObject(tmp,stack[skip],"own",infos);
			
			//childs
			var done = new Object;
			for (var j in stack)
			{
				var func = MattGetFunction(stack[j]);
				if (done[func] == undefined && !MattIsCPPOperator(func))
				{
					done[func] = true;
					MattReduceStackInfoObject(tmp,stack[j],"total",infos);
				}
			}
		}
		mattCache['flat.json'] = tmp;
	}

	//ok flush to user
	res.write(JSON.stringify(tmp,null,"\t"));
	res.end();
});

/****************************************************/
//export timed value to build charts
app.get('/timed.json',function(req,res) {
	var tmp = new Object();
	tmp.segments     = data.segments;
	tmp.internalMem  = data.internalMem;
	tmp.virtualMem   = data.virtualMem;
	tmp.physicalMem  = data.physicalMem;
	tmp.requestedMem = data.requestedMem;
	
	res.write(JSON.stringify(tmp,null));
	res.end();
});

/****************************************************/
app.get('/proc-map-distr.json',function(req,res) {
	var tmp = new Object();
	var map = data.stackInfo.sites.map;
	var checkStack = /^\[stack:[0-9]+\]$/;
	for (var i in map)
	{
		var mem = (parseInt(map[i].upper,16) - parseInt(map[i].lower,16));
		var file = map[i].file;
		if (file == '')
			file = 'anonymous';
		if (checkStack.test(file))
			file = 'stack';
		
		if (tmp[file] == undefined)
		{
			tmp[file] = {
				mem:mem,
				cnt:1
			};
		} else {
			tmp[file].mem += mem;
			tmp[file].cnt++;
		}
	}
	
	res.write(JSON.stringify(tmp,null,'\t'));
	res.end();
});

/****************************************************/
app.get('/',function(eq,res,next){
	res.render("stack-analysis",{});
});

/****************************************************/
app.get('/stack-analysis.html',function(eq,res,next){
	res.render("stack-analysis",{});
});

/****************************************************/
app.get('/time-analysis.html',function(eq,res,next){
	res.render("time-analysis",{});
});

/****************************************************/
//export static deps
app.use('/deps/jquery',Express.static(__dirname + '/client_deps/jquery-1.11.0'));
app.use('/deps/bootstrap',Express.static(__dirname + '/client_deps/bootstrap-3.1.1-dist'));
app.use('/deps/bootswatch/slate',Express.static(__dirname + '/client_deps/bootswatch.com/slate'));
app.use('/deps/bootswatch/fonts',Express.static(__dirname + '/client_deps/bootstrap-3.1.1-dist/fonts'));
app.use('/deps/ejs',Express.static(__dirname + '/client_deps/ejs-1.0/'));
app.use('/deps/ace',Express.static(__dirname + '/client_deps/ace-builds-1.1.1/'));
app.use('/deps/jqplot',Express.static(__dirname + '/client_deps/jqplot-1.0.8/'));
app.use('/deps/d3js',Express.static(__dirname + '/client_deps/d3js-3.4.2/'));
app.use('/deps/nvd3',Express.static(__dirname + '/client_deps/nvd3-1.1.15-beta/'));
app.use('/app-sources/',Express.static('/'));
app.use('/',Express.static(__dirname+'/client_files'));

/****************************************************/
//run express
console.log("Starting server on http://localhost:" + port);
app.listen(port,'localhost');
