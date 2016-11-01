/*****************************************************
             PROJECT  : MALT
             VERSION  : 2.0.0
             DATE     : 07/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/****************************************************/
//deps
var fs    = require('fs');
var clone = require('clone');
var path  = require('path');

/****************************************************/
function MaltProject(file)
{
	//declare internal stats to get a short list in same place
	this.data = null;//store data tree
	this.file = null;//more for debug to remember the related data file

	//load file
	if (file != undefined)
	{
		console.log("loading file "+file+"...");
		this.loadFile(file);
	}
}

/****************************************************/
MaltProject.prototype.loadData = function(data)
{
	//setup current data
	this.data = data;

// 	this.data.stacks = this.getFullTree();
// 	console.log(JSON.stringify(data));
// 	this.data = null;

	//optimize data
	console.log("Optimizing datas for requests...");
// 	optimizeProjectDatas(data);
	console.log("Data optimization done.");
}

/****************************************************/
/**
 * Function in charge of loading the json file.
**/
MaltProject.prototype.loadFile = function(file)
{
	//init
	this.data = null;
	this.file = file;
	var cur = this;

	//read from FS
	fs.readFile(args.params.input, 'utf8', function (err, buffer) {
		console.log("Loading file : " + args.params.input);

		//manage errors
		if (err) {
			console.error("Failed to open file "+file+", get error : "+err.code);
			process.exit(1);
// 			console.log(err);
// 			throw new Error(err);
		}

		//ok parse
		var data = JSON.parse(buffer);

		cur.loadData(data);
	});
}

/****************************************************/
//export definition
module.exports = MaltProject;
