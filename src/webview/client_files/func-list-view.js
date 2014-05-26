/********************************************************************/
/** Some constantes **/
var MATT_FUNC_LIST_TABLE_VIEW='/func-list-table-view.ejs';
var MATT_FUNC_LIST_LEFT_MENU='/func-list-menu-view.ejs';

/********************************************************************/
/** 
 * Class to implement the func list view 
 * @param containerId ID of the element in which to create the list view.
 * @param entryTemplate Provide the URL of the EJS file to use to render each entries.
 * @param funcListSelector profive a selector to filter the data to show in the view (must be MattFuncListSelector)
**/
function MattFuncListView(containerId,entryTemplate,funcListSelector)
{
	//check type
	mattHelper.assert(funcListSelector == undefined || funcListSelector instanceof MattFuncListSelector);
	mattHelper.assert(entryTemplate != undefined);
	
	//Store ID
	this.containerId = containerId;
	this.container = document.getElementById(containerId);
	mattHelper.assert(this.container != undefined);
	
	//keep track of URL for EJS template
	this.entryTemplate = entryTemplate;
	
	//register current object onto object
	this.container.matt = this;
	
	//setup default modes
	this.selector = null;
	if (funcListSelector != undefined)
	{
		this.selector = funcListSelector;
		var cur = this;
		funcListSelector.onChange = function() {cur.updateOptions(funcListSelector);};
	}
	
	//data not fetch now
	this.data = null;
	this.formattedData = null;
}

/********************************************************************/
/**
 * Declare render function to be used by user
**/
MattFuncListView.prototype.render = function()
{
	if (this.selector == null)
		throw Error("Not selector for rendering !");

	if (this.data == null)
	{
		this.internalFetchAndRender();
	} else {
		this.internalRender();
	}
}

/********************************************************************/
/**
 * Declare intenral fetchAndRender, for intneral use only.
**/
MattFuncListView.prototype.internalFetchAndRender = function()
{
	var mattObj = this;
	$.getJSON( "flat.json", function( data ) {
		mattObj.data = data;
		mattObj.internalRender();
	});
}

/********************************************************************/	
/**
 * Set a prefetch data instead for using the internal fetch method.
 * The data must be provided by the /flat.json URL. See format in wiki.
**/
MattFuncListView.prototype.setData = function(data)
{
	this.data = data;
	this.internalRender();
}

/********************************************************************/	
/**
 * Do the internal rendering by using the requested template.
**/
MattFuncListView.prototype.internalRender = function()
{
	this.selector.sanityCheck();
	var callback = new EJS({url: this.entryTemplate}).update(this.containerId);
	this.formattedData = this.selector.extractData(this.data);
	//alert(JSON.stringify(this.selector.getRange()));
	callback({data:this.formattedData,mattHelper:mattHelper,containerId:this.containerId,range:this.selector.getRange()});
}

/********************************************************************/	
/**
 * Manage the onclick signal by searching the info of the selected entry en 
 * call the user onClick method.
**/
MattFuncListView.prototype.clickEntry = function(id)
{
	this.onClick(this.formattedData.entries[id].details);
}

/********************************************************************/	
/**
 * To be override by user. It will be called when clicking on entries.
 * @param entry provide the detailed information on the selected entry.
**/
MattFuncListView.prototype.onClick = function(entry)
{	
	//alert(JSON.stringify(entry,null,'\t'));
}

/********************************************************************/
/**
 * Update the selector and refresh the view.
**/
MattFuncListView.prototype.updateOptions = function(options)
{
	this.selector = options;
	this.render();
}
