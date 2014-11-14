/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/****************************************************/
//Deps
var fs          = require('fs');
var Args        = require('arg-parser');
var Express     = require('express');
var clone       = require('clone');
var child       = require('child_process');

//internal classes
var MattProject = require('./server-files/MattProject.js');

/****************************************************/
//start express
var app = Express();

/****************************************************/
//intenral cache for computed data which take a while to built
var mattCache = new Object();

/****************************************************/
//Manage args
args = new Args('matt-webview', '1.0', 'Webiew for MATT based on Node.js','');
//define args
args.add({ name: 'input', desc: 'input file from MATT into JSON format', switches: [ '-i', '--input-file'], value: 'file', required: true });
args.add({ name: 'port',  desc: 'Port to use to wait for HTTP requests', switches: [ '-p', '--port'],       value: 'port', required: false });
args.add({ name: 'override',  desc: 'Override source dirs. Format is src1:dest1,src2:dest2...', switches: [ '-o', '--override'],       value: 'redirections', required: false });
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
var redirs = new Array();
if (args.params.override != undefined)
{
	var tmp = args.params.override.split(',');
	for (var i in tmp)
	{
		var tmp2 = tmp[i].split(':');
		redirs.push({source:tmp2[0],dest:tmp2[1]});
	}
}

/****************************************************/
//load file
var mattProject = new MattProject(args.params.input);

/****************************************************/
app.get('/flat.json',function(req,res) {
	var tmp = null;

	//check cache
	if (mattCache['flat.json'] != undefined)
	{
		//take from cache
		tmp = mattCache['flat.json'];
	} else {
		//compute and cache
		tmp = mattProject.getFlatFunctionProfile();
		mattCache['flat.json'] = tmp;
	}
	
	//ok flush to user
	res.write(JSON.stringify(tmp,null,"\t"));
	res.end();
});

/****************************************************/
app.get('/summary.json',function(req,res) {
	var tmp = mattProject.getSummary();
	res.write(JSON.stringify(tmp,null));
	res.end();
});

/****************************************************/
//export timed value to build charts
app.get('/timed.json',function(req,res) {
	var tmp = mattProject.getTimedValues();
	res.write(JSON.stringify(tmp,null));
	res.end();
});

/****************************************************/
//export max stack info
app.get('/max-stack-infos.json',function (req,res){
	var tmp = mattProject.getMaxStack();
	res.write(JSON.stringify(tmp,null));
	res.end();
});

/****************************************************/
//export max stack info
app.get('/stacks-mem.json',function (req,res){
	var tmp = mattProject.getStacksMem();
	res.write(JSON.stringify(tmp,null));
	res.end();
});

/****************************************************/
//export max stack info
app.get('/procmap.json',function (req,res){
	var tmp = mattProject.getProcMap();
	res.write(JSON.stringify(tmp,null));
	res.end();
});

/****************************************************/
//export max stack info
app.get('/global-variables.json',function (req,res){
	var tmp = mattProject.getGlobalVariables();
	res.write(JSON.stringify(tmp,null));
	res.end();
});

/****************************************************/
app.get('/stacks.json',function(req,res){
	//extratc file from request
	var file = req.query.file;
	var line = req.query.line;
	var func = req.query.func;
	
	//return error
	if (file != undefined || line != undefined)
	{
		console.log("extract stacks for : "+file+" +"+line);
		var tmp = mattProject.getFilterdStacksOnFileLine(file,line);
		res.write(JSON.stringify(tmp,null,"\t"));
	} else if (func != undefined) {
		console.log("extract stacks for : "+func);
		var tmp = mattProject.getFilterdStacksOnSymbol(func);
		res.write(JSON.stringify(tmp,null,"\t"));
	} else {
		res.send(500, 'Missing file or line GET parameter !');
	}
	res.end();
});

/****************************************************/
app.get('/memtrace-at.json',function(req,res) {
	console.log("At : "+req.query.at);
	console.log(mattProject.getTraceFilename());

	var at = req.query.at;
	var traceFile = mattProject.getTraceFilename();
	if (at == undefined)
	{
		res.send(500, 'Missing file GET parameter (at) !');
	} else if (traceFile == undefined) {
		res.send(500, 'Missing trace file for the given matt analysis !');
	} else {
		child.execFile('matt-trace-reader', [ traceFile, "--mem", "--filter", "at="+at], function(err, stdout, stderr) { 
			// Node.js will invoke this callback when the 
			console.log(stdout);
			var data = JSON.parse(stdout);
			var data = mattProject.completeMemtraceAt(data);
			res.write(JSON.stringify(data));
			res.end();
		});  
	}
});

/****************************************************/
app.get('/file-infos.json',function(req,res) {
	//extract file from request
	var file = req.query.file;
	
	//setup cache root entry
	if (mattCache['file-infos.json'] == undefined)
		mattCache['file-infos.json'] = new Object();
	
	//return error
	if (file == undefined)
	{
		res.send(500, 'Missing file GET parameter !');
	} else if (mattCache['file-infos.json'][file] != undefined) {
		res.write(mattCache['file-infos.json'][file]);
	} else {
		console.log("extract alloc info of file : "+file);
		var tmp = mattProject.getFileLinesFlatProfile(file,false);
		tmp = JSON.stringify(tmp,null,'\t');
		mattCache['file-infos.json'][file] = tmp;
		res.write(tmp);
	}
	res.end();
});

/****************************************************/
app.get('/max-stack.json',function(req,res) {
	var tmp = mattProject.getMaxStackInfoOnFunction();
	res.write(JSON.stringify(tmp,null,'\t'));
	res.end();
});

/****************************************************/
app.get('/stack.json',function(req,res) {
	var id = req.query.id;
	var tmp = mattProject.getStackInfoOnFunction(id);
	res.write(JSON.stringify(tmp,null,'\t'));
	res.end();
});

/****************************************************/
app.get('/proc-map-distr.json',function(req,res) {
	var tmp = mattProject.getProcMapDistr();
	res.write(JSON.stringify(tmp,null,'\t'));
	res.end();
});

/****************************************************/
app.get('/size-map.json',function(req,res) {
	var tmp = mattProject.getSizeMap();
	res.write(JSON.stringify(tmp,null,'\t'));
	res.end();
});

/****************************************************/
app.get('/realloc-map.json',function(req,res) {
	var tmp = mattProject.getReallocMap();
	res.write(JSON.stringify(tmp,null,'\t'));
	res.end();
});

/****************************************************/
app.get('/debug-stack-list.json',function(req,res) {
	var tmp = mattProject.getDebugStackList();
	res.write(JSON.stringify(tmp,null,'\t'));
	res.end();
});

/****************************************************/
app.get('/data/summary.json',function(req,res) {
	var tmp = mattProject.getSummaryV2();
	res.write(JSON.stringify(tmp,null,'\t'));
	res.end();
});

/****************************************************/
app.get('/',function(eq,res,next){
	res.redirect('app/index.html');
// 	res.render("page-summary",mattProject.getSummary());
});

/****************************************************/
app.get('/data.json',function(eq,res,next){
	res.sendfile(args.params.input);
});

/****************************************************/
var staticSourceServer = Express.static('/');
app.use('/app-sources/',function(req,res,next){
	
	var realPath = req.path;

	//check for redirect
	for (var i in redirs)
	{
		if (realPath.indexOf(redirs[i].source) == 0)
		{
			realPath = realPath.replace(redirs[i].source,redirs[i].dest)
			console.log("Apply redirection with override : " + req.path+" -> "+realPath);
		}
	}
	
	if (mattProject.isSourceFile(req.path))
	{
		console.log("Source file request :",realPath);
		req.path = realPath;
		req.url = realPath;
		return staticSourceServer(req,res,next);
	} else {
		//invalid source request
		console.log("Try to access invalid source file file :",realPath);
		res.send(404,"File not found");
	}
});

/****************************************************/
//export static deps
app.use('/',Express.static(__dirname+'/client_files'));
app.use('/app',Express.static(__dirname+'/client-files/app'));
app.use('/deps/angular',Express.static(__dirname+'/bower_components/angular'));
app.use('/deps/angular-route',Express.static(__dirname+'/bower_components/angular-route'));

// for (var i in redirs)
// {
// 	//TODO remove first '/' in strings
// 	console.log("override : " + redirs[i].source + " -> " + redirs[i].dest);
// 	app.use('/app-sources/'+redirs[i].source,Express.static('/'+redirs[i].dest));
// }

//console.log(JSON.stringify(mattProject.getFullTree(),null,'\t'));

/****************************************************/
//run express
console.log("\n\nStarting server on http://localhost:" + port+"\n\n");
app.listen(port,'localhost');
