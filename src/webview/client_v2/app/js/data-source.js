//SImple class to wrap all access to remote data, it might be easier to change or remove the server
//this way by just rewriting this class for exemple to directly load the data inside the browser
//itself
function MattDataSourceNodeJS()
{
	
}

//wrapper to fetch source files from remote server [source-editor.js]
MattDataSourceNodeJS.prototype.loadSourceFile = function(file,handler)
{
	$.get("/app-sources"+file,handler);
}

//wrapper to getch annotation of source files [source-editor.js]
MattDataSourceNodeJS.prototype.loadSourceFileAnnotations = function(file,handler)
{
	$.getJSON("/file-infos.json?file="+file,handler);
}

//wrapper to fetch timed data [page-timeline.js]
MattDataSourceNodeJS.prototype.loadTimedData = function($http,handler)
{
	$http.get('/timed.json').success(handler);
}

//wrapper to fetch stack data [page-stack-peak.js]
MattDataSourceNodeJS.prototype.loadStackData = function($http,handler)
{
	$http.get('/stacks-mem.json').success(handler);
}

//wrapper to getch data for function list on left side of source browser [page-source.js & page-home]
MattDataSourceNodeJS.prototype.loadFlatFunctionStats = function($http,handler)
{
	$http.get('/flat.json').success(handler);
}

//wrapper to fetch data for realloc stats [page-realloc.js]
MattDataSourceNodeJS.prototype.loadReallocStats = function($http,handler)
{
	$http.get('/realloc-map.json').success(handler);
}

//wrapper to fetch global summary for home page [page-home.js]
MattDataSourceNodeJS.prototype.loadGlobalSummary = function($http,handler)
{
	$http.get('/data/summary.json').success(handler);
}

//wrapper to fetch the global variables infos [page-global-vars.js]
MattDataSourceNodeJS.prototype.loadGlobalVars = function($http,handler)
{
	$http.get('/global-variables.json').success(handler);
}

MattDataSourceNodeJS.prototype.getSizeDistr = function($http,handler)
{
	$http.get('/size-map.json').success(handler);
}

MattDataSourceNodeJS.prototype.getCallStackDataFileLine = function(file,line,handler)
{
	$.getJSON("/stacks.json?file="+encodeURIComponent(file)+"&line="+line,handler);
}

MattDataSourceNodeJS.prototype.getCallStackDataFunc = function(func,handler)
{
	$.getJSON("/stacks.json?func="+encodeURIComponent(func),handler);
}

/////////////////////////////////////////////////////////////////////////

//SImple class to wrap all access to remote data, it might be easier to change or remove the server
//this way by just rewriting this class for exemple to directly load the data inside the browser
//itself
function MattDataSourceClientSide()
{
	var cur = this;
	this.cache = {};
	this.data = {};
// 	var MattProject = require("MattProject");
	this.project = new MattProject();
	
	$.getJSON("/data.json",function(data) {
		cur.data = data;
		cur.project.loadData(data);
	});
}

//wrapper to fetch source files from remote server [source-editor.js]
MattDataSourceClientSide.prototype.loadSourceFile = function(file,handler)
{
	$.get("/app-sources"+file,handler);
}

//wrapper to getch annotation of source files [source-editor.js]
MattDataSourceClientSide.prototype.loadSourceFileAnnotations = function(file,handler)
{
	$.getJSON("/file-infos.json?file="+file,handler);
}

//wrapper to fetch timed data [page-timeline.js]
MattDataSourceClientSide.prototype.loadTimedData = function($http,handler)
{
// 	$http.get('/timed.json').success(handler);
	handler(this.project.getTimedValues());
}

//wrapper to fetch stack data [page-stack-peak.js]
MattDataSourceClientSide.prototype.loadStackData = function($http,handler)
{
// 	$http.get('/stacks-mem.json').success(handler);
	handler(this.project.getStacksMem());
}

//wrapper to getch data for function list on left side of source browser [page-source.js & page-home]
MattDataSourceClientSide.prototype.loadFlatFunctionStats = function($http,handler)
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
MattDataSourceClientSide.prototype.loadReallocStats = function($http,handler)
{
// 	$http.get('/realloc-map.json').success(handler);
	handler(this.project.getReallocMap());
}

//wrapper to fetch global summary for home page [page-home.js]
MattDataSourceClientSide.prototype.loadGlobalSummary = function($http,handler)
{
// 	$http.get('/data/summary.json').success(handler);
	handler(this.project.getSummaryV2());
}

//wrapper to fetch the global variables infos [page-global-vars.js]
MattDataSourceClientSide.prototype.loadGlobalVars = function($http,handler)
{
// 	$http.get('/global-variables.json').success(handler);
	handler(this.project.getGlobalVariables());
}

MattDataSourceClientSide.prototype.getSizeDistr = function($http,handler)
{
// 	$http.get('/size-map.json').success($http,handler);
	handler(this.project.getSizeMap());
}

MattDataSourceClientSide.prototype.getCallStackDataFileLine = function(file,line,handler)
{
	//$.getJSON("/stacks.json?file="+encodeURIComponent(file)+"&line="+line,handler);
	handler(this.project.getFilterdStacksOnFileLine(file,line));
}

MattDataSourceClientSide.prototype.getCallStackDataFunc = function(func,handler)
{
	//$.getJSON("/stacks.json?func="+encodeURIComponent(func),handler);
	handler(this.project.getFilterdStacksOnSymbol(func));
}

/////////////////////////////////////////////////////////////////////////

//create global instance to be used in all pages
var mattDataSource = new MattDataSourceNodeJS();
// var mattDataSource = new MattDataSourceClientSide();
