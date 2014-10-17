//SImple class to wrap all access to remote data, it might be easier to change or remove the server
//this way by just rewriting this class for exemple to directly load the data inside the browser
//itself
function MattDataSource()
{
	
}

//wrapper to fetch source files from remote server [source-editor.js]
MattDataSource.prototype.loadSourceFile = function(file,handler)
{
	$.get("/app-sources"+file,handler);
}

//wrapper to getch annotation of source files [source-editor.js]
MattDataSource.prototype.loadSourceFileAnnotations = function(file,handler)
{
	$.getJSON("/file-infos.json?file="+file,handler);
}

//wrapper to fetch timed data [page-timeline.js]
MattDataSource.prototype.loadTimedData = function($http,handler)
{
	$http.get('/timed.json').success(handler);
}

//wrapper to fetch stack data [page-stack-peak.js]
MattDataSource.prototype.loadStackData = function($http,handler)
{
	$http.get('/stacks-mem.json').success(handler);
}

//wrapper to getch data for function list on left side of source browser [page-source.js & page-home]
MattDataSource.prototype.loadFlatFunctionStats = function($http,handler)
{
	$http.get('/flat.json').success(handler);
}

//wrapper to fetch data for realloc stats [page-realloc.js]
MattDataSource.prototype.loadReallocStats = function($http,handler)
{
	$http.get('/realloc-map.json').success(handler);
}

//wrapper to fetch global summary for home page [page-home.js]
MattDataSource.prototype.loadGlobalSummary = function($http,handler)
{
	$http.get('/data/summary.json').success(handler);
}

//wrapper to fetch the global variables infos [page-global-vars.js]
MattDataSource.prototype.loadGlobalVars = function($http,handler)
{
	$http.get('/global-variables.json').success(handler);
}

MattDataSource.prototype.getSizeDistr = function($http,handler)
{
	$http.get('/size-map.json').success($http,handler);
}

//create global instance to be used in all pages
var mattDataSource = new MattDataSource();
