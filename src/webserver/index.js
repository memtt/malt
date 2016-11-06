/*****************************************************
             PROJECT  : MALT
             VERSION  : 2.0.0
             DATE     : 07/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/****************************************************/
//Deps
var fs = require('fs');
var Args = require('arg-parser');
var Express = require('express');
var clone = require('clone');
var child = require('child_process');
var auth = require('http-auth');

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
args = new Args('malt-webserver', '1.0', 'Webiew for MALT based on Node.js', '');
//define args
args.add({ name: 'input', desc: 'input file from MALT into JSON format', switches: ['-i', '--input-file'], value: 'file', required: true });
args.add({ name: 'port', desc: 'Port to use to wait for HTTP requests', switches: ['-p', '--port'], value: 'port', required: false });
args.add({ name: 'override', desc: 'Override source dirs. Format is src1:dest1,src2:dest2...', switches: ['-o', '--override'], value: 'redirections', required: false });
args.add({ name: 'noauth', desc: 'Disable http authentification', switches: ['-n', '--no-auth'], required: false });
if (!args.parse()) {
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
if (args.params.noauth == undefined) {
	//log
	console.log("Load http auth from ~/.malt/passwd, you can change your passwod with 'malt-passwd {user}' or disable auth with --no-auth\n");

	//setup auth system
	var basic = auth.basic({
		realm: "MALT web GUI.",
		file: getUserHome() + "/.malt/passwd"
	});
	//inserto into express
	app.use(auth.connect(basic));
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
var maltProject = new MaltProject(args.params.input);

/****************************************************/
app.get('/api/run-infos.json',function(req,res) {
	res.write(JSON.stringify(maltProject.getRunInfos(),null,"\t"));
	res.end();
});

/****************************************************/
app.get('/api/summary-infos.json',function(req,res) {
	res.write(JSON.stringify(maltProject.getSummaryInfos(),null,"\t"));
	res.end();
});

/****************************************************/
app.get('/api/thread-stats.json',function(req,res) {
	res.write(JSON.stringify(maltProject.getThreadStats(),null,"\t"));
	res.end();
});

/****************************************************/
//export static deps
app.use('/',Express.static(__dirname+'/../webview/dist'));
app.use('/deps/',Express.static(__dirname+'/bower_components/'));

/****************************************************/
//run express
console.log("\n\nStarting server on http://localhost:" + port+"\n\n");
app.listen(port,'localhost');
