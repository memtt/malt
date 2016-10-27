//SImple class to wrap all access to remote data, it might be easier to change or remove the server
//this way by just rewriting this class for exemple to directly load the data inside the browser
//itself
function MaltDataSourceNodeJS()
{
	
}

//wrapper to fetch source files from remote server [source-editor.js]
MaltDataSourceNodeJS.prototype.loadSourceFile = function(file,handler,fail)
{
	$.get("/app-sources"+file)
		.done(handler)
		.fail(fail);
}

//wrapper to getch annotation of source files [source-editor.js]
MaltDataSourceNodeJS.prototype.loadSourceFileAnnotations = function(file,handler)
{
	$.getJSON("/file-infos.json?file="+file,handler);
}

//wrapper to fetch timed data [page-timeline.js]
MaltDataSourceNodeJS.prototype.loadTimedData = function($http,handler)
{
	$http.get('/timed.json').success(handler);
}

//wrapper to fetch timed data [page-alloc-size-distr.js]
MaltDataSourceNodeJS.prototype.getScatter = function($http,handler)
{
	$http.get('/scatter.json').success(handler);
}

//wrapper to fetch stack data [page-stack-peak.js]
MaltDataSourceNodeJS.prototype.loadStackData = function($http,handler)
{
	$http.get('/stacks-mem.json').success(handler);
}

//wrapper to getch data for function list on left side of source browser [page-source.js & page-home]
MaltDataSourceNodeJS.prototype.loadFlatFunctionStats = function($http,handler)
{
	$http.get('/flat.json').success(handler);
}

//wrapper to fetch data for realloc stats [page-realloc.js]
MaltDataSourceNodeJS.prototype.loadReallocStats = function($http,handler)
{
	$http.get('/realloc-map.json').success(handler);
}

//wrapper to fetch global summary for home page [page-home.js]
MaltDataSourceNodeJS.prototype.loadGlobalSummary = function($http,handler)
{
	$http.get('/data/summary.json').success(handler);
}

//wrapper to fetch the global variables infos [page-global-vars.js]
MaltDataSourceNodeJS.prototype.loadGlobalVars = function($http,handler)
{
	$http.get('/global-variables.json').success(handler);
}

MaltDataSourceNodeJS.prototype.getSizeDistr = function($http,handler)
{
	$http.get('/size-map.json').success(handler);
}

MaltDataSourceNodeJS.prototype.getCallStackDataFileLine = function(file,line,handler)
{
	$.getJSON("/stacks.json?file="+encodeURIComponent(file)+"&line="+line,handler);
}

MaltDataSourceNodeJS.prototype.getCallStackDataFunc = function(func,handler)
{
	$.getJSON("/stacks.json?func="+encodeURIComponent(func),handler);
}

MaltDataSourceNodeJS.prototype.getCallTreeData = function(nodeid, depth, height, mincost, func, metric, isRatio, handler, fail)
{
	$.getJSON("/calltree?"
		+ (func ? "func=" + encodeURIComponent(func) : "nodeid=" + encodeURIComponent(nodeid))
		+ "&depth=" + encodeURIComponent(depth)
		+ "&height=" + encodeURIComponent(height)
		+ "&mincost=" + encodeURIComponent(mincost)
		+ "&metric=" + encodeURIComponent(metric)
		+ "&isratio=" + encodeURIComponent(isRatio))
		.done(handler)
		.fail(fail);
}

MaltDataSourceNodeJS.prototype.getCallTreeFormatLink = function(nodeid, depth, height, mincost, func, metric, isRatio, format) {
	return  "/calltree?"
		+ (func ? "func=" + encodeURIComponent(func) : "nodeid=" + encodeURIComponent(nodeid))
		+ "&depth=" + encodeURIComponent(depth)
		+ "&height=" + encodeURIComponent(height)
		+ "&mincost=" + encodeURIComponent(mincost)
		+ "&metric=" + encodeURIComponent(metric)
		+ "&isratio=" + encodeURIComponent(isRatio)
		+ "&format=" + encodeURIComponent(format);
}

MaltDataSourceNodeJS.prototype.loadProcMaps = function(func,handler)
{
	$.getJSON("/procmaps.json",handler);
}

MaltDataSourceNodeJS.prototype.getActiveChunks = function(timestamp, handler)
{
	$.getJSON("/active-chunks?timestamp=" + encodeURIComponent(timestamp), handler);
}

/////////////////////////////////////////////////////////////////////////

//SImple class to wrap all access to remote data, it might be easier to change or remove the server
//this way by just rewriting this class for exemple to directly load the data inside the browser
//itself
function MaltDataSourceClientSide()
{
	var cur = this;
	this.cache = {};
	this.data = {};
// 	var MaltProject = require("MaltProject");
	this.project = new MaltProject();
	
	$.getJSON("/data.json",function(data) {
		cur.data = data;
		cur.project.loadData(data);
	});
}

//SImple class to wrap all access to remote data, it might be easier to change or remove the server
//this way by just rewriting this class for exemple to directly load the data inside the browser
//itself
function MaltDataSourceClientSideData(data)
{
	var cur = this;
	this.cache = {};
	this.data = {};
// 	var MaltProject = require("MaltProject");
	this.project = new MaltProject();
	
	cur.project.loadData(data);
}

//wrapper to fetch source files from remote server [source-editor.js]
MaltDataSourceClientSide.prototype.loadSourceFile = function(file,handler)
{
	$.get("/app-sources"+file,handler);
}

//wrapper to getch annotation of source files [source-editor.js]
MaltDataSourceClientSide.prototype.loadSourceFileAnnotations = function(file,handler)
{
	$.getJSON("/file-infos.json?file="+file,handler);
}

//wrapper to fetch timed data [page-timeline.js]
MaltDataSourceClientSide.prototype.loadTimedData = function($http,handler)
{
// 	$http.get('/timed.json').success(handler);
	handler(this.project.getTimedValues());
}

//wrapper to fetch stack data [page-stack-peak.js]
MaltDataSourceClientSide.prototype.loadStackData = function($http,handler)
{
// 	$http.get('/stacks-mem.json').success(handler);
	handler(this.project.getStacksMem());
}

//wrapper to getch data for function list on left side of source browser [page-source.js & page-home]
MaltDataSourceClientSide.prototype.loadFlatFunctionStats = function($http,handler)
{
	//$http.get('/flat.json').success(handler);
	var res = this.cache['/flat.json'];
	if (res == undefined)
	{
		res = this.project.getFlatFunctionProfile();
		this.cache['/flat.json'] = res;
	}
	handler(res);
}

//wrapper to fetch data for realloc stats [page-realloc.js]
MaltDataSourceClientSide.prototype.loadReallocStats = function($http,handler)
{
// 	$http.get('/realloc-map.json').success(handler);
	handler(this.project.getReallocMap());
}

//wrapper to fetch global summary for home page [page-home.js]
MaltDataSourceClientSide.prototype.loadGlobalSummary = function($http,handler)
{
// 	$http.get('/data/summary.json').success(handler);
	handler(this.project.getSummaryV2());
}

//wrapper to fetch the global variables infos [page-global-vars.js]
MaltDataSourceClientSide.prototype.loadGlobalVars = function($http,handler)
{
// 	$http.get('/global-variables.json').success(handler);
	handler(this.project.getGlobalVariables());
}

MaltDataSourceClientSide.prototype.getSizeDistr = function($http,handler)
{
// 	$http.get('/size-map.json').success($http,handler);
	handler(this.project.getSizeMap());
}

MaltDataSourceClientSide.prototype.getScatter = function($http,handler)
{
// 	$http.get('/scatter.json').success($http,handler);
	handler(this.project.getScatter());
}

MaltDataSourceClientSide.prototype.getCallStackDataFileLine = function(file,line,handler)
{
	//$.getJSON("/stacks.json?file="+encodeURIComponent(file)+"&line="+line,handler);
	handler(this.project.getFilterdStacksOnFileLine(file,line));
}

MaltDataSourceClientSide.prototype.getCallStackDataFunc = function(func,handler)
{
	//$.getJSON("/stacks.json?func="+encodeURIComponent(func),handler);
	handler(this.project.getFilterdStacksOnSymbol(func));
}

/////////////////////////////////////////////////////////////////////////
if (maltStandalone)
{
	var maltDataSource = new MaltDataSourceClientSide();
	// var maltDataSource = new MaltDataSourceClientSideData(maltData);
} else {
	var maltDataSource = new MaltDataSourceNodeJS();
}
