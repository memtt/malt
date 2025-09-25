/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 03/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/malt-simple-dump.js
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
//Deps
var Args        = require('arg-parser');

//internal classes
var MaltProject = require('./server-files/MaltProject.js');

/**********************************************************/
//Manage args
args = new Args('malt-simple-dumper', '1.0', 'Simply dump part of the json profile to validate them in unit tests.','');
//define args
args.add({ name: 'input', desc: 'input file from MALT into JSON format', switches: [ '-i', '--input-file'], value: 'file', required: true });
args.add({ name: 'source_file', desc: 'Source file to extract', switches: [ '-s', '--source-file'], value: 'file', required: true });
if (!args.parse())
{
	console.error("Invalid parameters, please check with -h");
	process.exit(1);
}

/**********************************************************/
//load file
var maltProject = new MaltProject(args.params.input, function() {
	console.log("----------------- EXTRACT ------------------");
	const file = args.params.input;
	const source_file = args.params.source_file;
	console.log(file);
	console.log(source_file);
	console.log("--------------------------------------------");
	/*var tmp = maltProject.getFileLinesFlatProfile(source_file, true);
	tmp.forEach((value) => {
		if (value.total != undefined) {
			var d = JSON.stringify(value.total);
			var fname = source_file.split("/");
			fname = fname[fname.length - 1];
			console.log(`\n${fname}:${value.function}:${value.line}`);
			console.log(` - alloc: ${JSON.stringify(value.total.alloc)}`);
			console.log(` - free: ${JSON.stringify(value.total.alloc)}`);
			console.log(` - globalPeak: ${value.total.globalPeak}`);
		}
	});*/
});
