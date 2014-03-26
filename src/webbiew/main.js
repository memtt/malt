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
//internal classes
var MattProject = require('./server_files/lib/MattProject.js');

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
var data = null;
console.log("Loading file : " + args.params.input);
fs.readFile(args.params.input, 'utf8', function (err, buffer) {
	if (err) {
		console.log('Error: ' + err);
		return;
	}
	data = JSON.parse(buffer);
});

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
//export timed value to build charts
app.get('/timed.json',function(req,res) {
	var tmp = mattProject.getTimedValues();
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
app.get('/proc-map-distr.json',function(req,res) {
	var tmp = mattProject.getProcMapDistr();
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
app.get('/max-stack.html',function(eq,res,next){
	res.render("max-stack",{});
});


/****************************************************/
//export static deps
app.use('/deps/jquery',Express.static(__dirname + '/client_deps/jquery-1.11.0'));
app.use('/deps/jquery-treetable',Express.static(__dirname + '/client_deps/jquery-treetable-3.1.0/'));
app.use('/deps/bootstrap',Express.static(__dirname + '/client_deps/bootstrap-3.1.1-dist'));
app.use('/deps/bootswatch/slate',Express.static(__dirname + '/client_deps/bootswatch.com/slate'));
app.use('/deps/bootswatch/fonts',Express.static(__dirname + '/client_deps/bootstrap-3.1.1-dist/fonts'));
app.use('/deps/ejs',Express.static(__dirname + '/client_deps/ejs-1.0/'));
app.use('/deps/ace',Express.static(__dirname + '/client_deps/ace-builds-1.1.1/'));
app.use('/deps/jqplot',Express.static(__dirname + '/client_deps/jqplot-1.0.8/'));
app.use('/deps/d3js',Express.static(__dirname + '/client_deps/d3js-3.4.2/'));
app.use('/deps/nvd3',Express.static(__dirname + '/client_deps/nvd3-1.1.15-beta/'));
app.use('/deps/codemirror/lib',Express.static(__dirname + '/node_modules/codemirror/lib'));
app.use('/deps/codemirror/theme',Express.static(__dirname + '/node_modules/codemirror/theme'));
app.use('/deps/codemirror/mode',Express.static(__dirname + '/node_modules/codemirror/mode'));
app.use('/deps/codemirror/addon',Express.static(__dirname + '/node_modules/codemirror/addon'));
app.use('/app-sources/',Express.static('/'));
app.use('/',Express.static(__dirname+'/client_files'));

for (var i in redirs)
{
	//TODO remove first '/' in strings
	console.log("override : " + redirs[i].source + " -> " + redirs[i].dest);
	app.use('/app-sources/'+redirs[i].source,Express.static('/'+redirs[i].dest));
}

/****************************************************/
//run express
console.log("Starting server on http://localhost:" + port);
app.listen(port,'localhost');
