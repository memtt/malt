/********************************************************************/
/**
 * Provide a simple text zone to display details when clicking on source code annotations to get 
 * detailed info about the location.
 * @param containerId Provide the ID of the div in which to setup the code editor.
**/
function MattCallSiteDetailView(containerId,selector)
{
	//setup container
	this.containerId = containerId;
	this.container = document.getElementById(containerId);
	
	//check and link
	mattHelper.assert(this.container != undefined);
	this.container.matt = this;
}

/********************************************************************/
MattCallSiteDetailView.prototype.render = function(info)
{
	var callback = new EJS({url: 'call-site-detail-view.ejs'}).update(this.containerId);
	callback({info:info});
}
