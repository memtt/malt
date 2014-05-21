/** Class to implement the func list view **/
function MattFuncListView(ulId,funcListSelector)
{
	//Store ID
	this.ulId = ulId;
	this.ul = document.getElementById(ulId);
	
	//register current object onto object
	this.ul.matt = this;
	
	//setup default modes
	this.options = null;
	if (funcListSelector != undefined)
	{
		this.options = funcListSelector;
		var cur = this;
		funcListSelector.onChange = function() {cur.updateOptions(funcListSelector);};
	}
	
	//data not fetch now
	this.data = null;
	this.formattedData = null;
	
	//Declare render function
	this.render = function()
	{
		if (this.options == null)
			throw Error("Not option for rendering !");
		if (this.data == null)
		{
			this.internalFetchAndRender();
		} else {
			this.internalRender();
		}
	}
	
	//Declare intenral fetchAndRender
	this.internalFetchAndRender = function()
	{
		var mattObj = this;
		$.getJSON( "flat.json", function( data ) {
			mattObj.data = data;
			mattObj.internalRender();
		});
	}
	
	//setup data from user
	this.setData = function(data)
	{
		this.data = data;
		this.internalRender();
	}

	//Declare internal render
	this.internalRender = function()
	{
		this.options.sanityCheck();
		var callback = new EJS({url: '/func-list/func-list-table-view.ejs'}).update(this.ulId);
		this.formattedData = this.options.extractData(this.data);
		callback(this.formattedData);
	}
	
	this.clickEntry = function(id)
	{
		this.onClick(this.formattedData.data.entries[id].details);
	}
	
	//to override
	this.onClick = function(details)
	{	
	}
	
	this.updateOptions = function(options)
	{
		this.options = options;
		this.render();
	}
}