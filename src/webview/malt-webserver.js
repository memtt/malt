/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
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
var auth        = require('http-auth');

//internal classes
var MaltProject = require('./server-files/MaltProject.js');

/****************************************************/
//start express
var app = Express();

/****************************************************/
//intenral cache for computed data which take a while to built
var maltCache = new Object();

/****************************************************/
//Manage args
args = new Args('malt-webserver', '1.0', 'Webiew for MALT based on Node.js','');
//define args
args.add({ name: 'input', desc: 'input file from MALT into JSON format', switches: [ '-i', '--input-file'], value: 'file', required: true });
args.add({ name: 'host',  desc: 'Host to bind to locally', switches: [ '-H', '--host'], value: 'host', required: false });
args.add({ name: 'port',  desc: 'Port to use to wait for HTTP requests', switches: [ '-p', '--port'],       value: 'port', required: false });
args.add({ name: 'override',  desc: 'Override source dirs. Format is src1:dest1,src2:dest2...', switches: [ '-o', '--override'],       value: 'redirections', required: false });
args.add({ name: 'noauth',    desc: 'Disable http authentification', switches: ['-n', '--no-auth'], required: false });
args.add({ name: 'authfile',    desc: 'Use custom authentification file', switches: ['-A', '--authfile'], value:'file', required: false });
if (!args.parse()) 
{
	console.error("Invalid parameters, please check with -h");
	process.exit(1);
}

/****************************************************/
//Get home dir
function getUserHome() {
	return process.env[(process.platform == 'win32') ? 'USERPROFILE' : 'HOME'];
}

/****************************************************/
//Setup http auth if enabled
if (args.params.noauth == undefined)
{
	//fil
	var file = getUserHome() + "/.malt/passwd";
	if (args.params.authfile != undefined)
		file = args.params.authfile;
	
	//log
	console.log("Load http auth from "+file+", you can change your passwod with 'malt-passwd {user}' or disable auth with --no-auth\n");

	//setup auth system
	var basic = auth.basic({
	    realm: "MALT web GUI.",
	    file: file
	});
	//inserto into express
	app.use(auth.connect(basic));
}

/****************************************************/
var host = 'localhost';
if (args.params.host != undefined)
	host = args.params.host;

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
var maltProject = new MaltProject(args.params.input);

/****************************************************/
app.get('/flat.json',function(req,res) {
	var tmp = null;

	//check cache
	if (maltCache['flat.json'] != undefined)
	{
		//take from cache
		tmp = maltCache['flat.json'];
	} else {
		//compute and cache
		tmp = maltProject.getFlatFunctionProfile();
		maltCache['flat.json'] = tmp;
	}
	
	//ok flush to user
	res.json(tmp);
	res.end();
});

/****************************************************/
app.get('/summary.json',function(req,res) {
	var tmp = maltProject.getSummary();
	res.json(tmp);
	res.end();
});

/****************************************************/
//export timed value to build charts
app.get('/timed.json',function(req,res) {
	var tmp = maltProject.getTimedValues();
	res.json(tmp);
	res.end();
});

/****************************************************/
//export max stack info
app.get('/max-stack-infos.json',function (req,res){
	var tmp = maltProject.getMaxStack();
	res.json(tmp);
	res.end();
});

/****************************************************/
//export max stack info
app.get('/stacks-mem.json',function (req,res){
	var tmp = maltProject.getStacksMem();
	res.json(tmp);
	res.end();
});

/****************************************************/
//export max stack info
app.get('/procmap.json',function (req,res){
	var tmp = maltProject.getProcMap();
	res.json(tmp);
	res.end();
});

/****************************************************/
//export max stack info
app.get('/global-variables.json',function (req,res){
	var tmp = maltProject.getGlobalVariables();
	res.json(tmp);
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
		var tmp = maltProject.getFilterdStacksOnFileLine(file,line);
		res.json(tmp);
	} else if (func != undefined) {
		console.log("extract stacks for : "+func);
		var tmp = maltProject.getFilterdStacksOnSymbol(func);
		res.json(tmp);
	} else {
		res.send(500, 'Missing file or line GET parameter !');
	}
	res.end();
});

/****************************************************/
app.get('/memtrace-at.json',function(req,res) {
	console.log("At : "+req.query.at);
	console.log(maltProject.getTraceFilename());

	var at = req.query.at;
	var traceFile = maltProject.getTraceFilename();
	if (at == undefined)
	{
		res.send(500, 'Missing file GET parameter (at) !');
	} else if (traceFile == undefined) {
		res.send(500, 'Missing trace file for the given malt analysis !');
	} else {
		child.execFile('malt-trace-reader', [ traceFile, "--mem", "--filter", "at="+at], function(err, stdout, stderr) { 
			// Node.js will invoke this callback when the 
			console.log(stdout);
			var data = JSON.parse(stdout);
			var data = maltProject.completeMemtraceAt(data);
			res.json(data);
			res.end();
		});  
	}
});

/****************************************************/
app.get('/file-infos.json',function(req,res) {
	//extract file from request
	var file = req.query.file;
	
	//setup cache root entry
	if (maltCache['file-infos.json'] == undefined)
		maltCache['file-infos.json'] = new Object();
	
	//return error
	if (file == undefined)
	{
		res.send(500, 'Missing file GET parameter !');
	} else if (maltCache['file-infos.json'][file] != undefined) {
		res.type('json');
		res.write(maltCache['file-infos.json'][file]);
	} else {
		console.log("extract alloc info of file : "+file);
		var tmp = maltProject.getFileLinesFlatProfile(file,false);
		tmp = JSON.stringify(tmp,null,'\t');
		maltCache['file-infos.json'][file] = tmp;
		res.type('json');
		res.write(tmp);
	}
	res.end();
});

/****************************************************/
app.get('/max-stack.json',function(req,res) {
	var tmp = maltProject.getMaxStackInfoOnFunction();
	res.json(tmp);
	res.end();
});

/****************************************************/
app.get('/stack.json',function(req,res) {
	var id = req.query.id;
	var tmp = maltProject.getStackInfoOnFunction(id);
	res.json(tmp);
	res.end();
});

/****************************************************/
app.get('/proc-map-distr.json',function(req,res) {
	var tmp = maltProject.getProcMapDistr();
	res.json(tmp);
	res.end();
});

/****************************************************/
//export scatter info
app.get('/scatter.json',function (req,res){
	var tmp = maltProject.getScatter();
	res.json(tmp);
	res.end();
});

/****************************************************/
app.get('/size-map.json',function(req,res) {
	var tmp = maltProject.getSizeMap();
	res.json(tmp);
	res.end();
});

/****************************************************/
app.get('/realloc-map.json',function(req,res) {
	var tmp = maltProject.getReallocMap();
	res.json(tmp);
	res.end();
});

/****************************************************/
app.get('/debug-stack-list.json',function(req,res) {
	var tmp = maltProject.getDebugStackList();
	res.json(tmp);
	res.end();
});

/****************************************************/
app.get('/data/summary.json',function(req,res) {
	var tmp = maltProject.getSummaryV2();
	res.json(tmp);
	res.end();
});

app.get('/calltree', function(req, res) {
	maltProject.getCallTree(req.query.nodeid, req.query.depth, 
		req.query.height, req.query.mincost, req.query.func, req.query.metric, 
		req.query.isratio, function(data) {
			if(req.query.format == 'svg') {
				res.header("Content-Type", "image/svg+xml");
				res.setHeader('Content-disposition', 'attachment; filename=calltree.svg');
				res.send(data.svg);
				res.end();
			} else if (req.query.format == 'dot') {
				res.header("Content-Type", "text/vnd.graphviz");
				res.setHeader('Content-disposition', 'attachment; filename=calltree.dot');
				res.send(data.dotCode);
				res.end();				
			} else {
				res.json(data);
			}
		});
});

app.get('/active-chunks', function(req, res) {
	maltProject.getActiveChunks(req.query.timestamp, function(result) {
		res.json(result);
	});
});

/****************************************************/
app.get('/',function(eq,res,next){
	res.redirect('app/index.html');
// 	res.render("page-summary",maltProject.getSummary());
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
	
	if (maltProject.isSourceFile(req.path))
	{
		console.log("Source file request :",realPath);
		req.path = realPath;
		req.url = realPath;
		res.header("Content-Type", "text/plain");
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

//console.log(JSON.stringify(maltProject.getFullTree(),null,'\t'));

/****************************************************/
//run express
console.log("\n\nStarting server on http://"+host+":" + port+"\n\n");
app.listen(port,host);
