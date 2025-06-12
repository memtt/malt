/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 04/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/app/js/call-stack-view.js
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
*    AUTHOR   : Mehdi Raza Jaffery (CERN) - 2016
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/
/********************************************************************/
/**
 * Provide a component to display stacks annotated with the current metric selected with selector.
 * @param containerId Define the DIV ID in which to draw the tree.
 * @param selector Define the selector object to be used to extract informations to annotate the tree.
**/
function MaltCallStacksView(containerId,selector)
{
	//errors
	//maltHelper.assert(selector != undefined && selector instanceof MaltFuncListSelector);
	
	//setup container
	this.containerId = containerId;
	this.container = $('#'+containerId);
	
	//check and link
	//maltHelper.assert(this.container != undefined);
	this.container.malt = this;
	
	//defaults
	this.stackViewRoots = [];
	this.tree = null;
	this.selector = selector;
	this.filterByLocation = {};
	
	//counter to avoid multiple update due to async
	this.asyncState = 0;
	
	//setup
	this.initRender();
}

/********************************************************************/
MaltCallStacksView.prototype.initRender = function()
{
	var cur = this;
	this.container.treetable({ 
		expandable: true,
		initialState:'collasped',
		clickableNodeNames:false,
		onNodeExpand: function() {
			var node = this;
			maltDataSource.getCallStackLevel(node.maltData.stackId, node.maltData.stackDepth, cur.filterByLocation, function(data) {
				cur.addToTree(data, 0, 0);
			});
			//cur.addToTree(node.maltData);
        },
		onNodeCollapse: function() {
			var node = this;
			cur.container.treetable("unloadBranch", node);
		},
	});
}

/********************************************************************/
MaltCallStacksView.prototype.clear = function()
{
	var cur = this;
	this.stackViewRoots.forEach(function(value){
		cur.container.treetable('removeNode',value);
	});
	this.stackViewRoots = [];
}

/********************************************************************/
MaltCallStacksView.prototype.update = function(file,line)
{
	var cur = this;
	this.clear();
	
	//avoid overlap between async req
	this.asyncState++;
	var state = this.asyncState;
	this.filterByLocation = {"file": file, "line": line};

	/*maltDataSource.getCallStackDataFileLine(file,line,function(data) {
		if (state == cur.asyncState)
		{
			var tree = cur.buildCallTree(data);
			cur.addToTree(tree);
			cur.tree = tree;
		}
	});*/

	maltDataSource.getCallStackLevel(0, 0, this.filterByLocation, function(data) {
		if (state == cur.asyncState)
		{
			cur.addToTree(data, 0, 0);
		}
	});
}

/********************************************************************/
MaltCallStacksView.prototype.updateFunc = function(func)
{
	var cur = this;
	this.clear();
	
	//avoid overlap between async req
	this.asyncState++;
	var state = this.asyncState;
	this.filterByLocation = {"function": func};

	/*maltDataSource.getCallStackDataFunc(func,function(data) {
		if (state == cur.asyncState)
		{
			var tree = cur.buildCallTree(data);
			cur.addToTree(tree);
			cur.tree = tree;

		}
	});*/

	maltDataSource.getCallStackLevel(0, 0, this.filterByLocation, function(data) {
		if (state == cur.asyncState)
		{
			cur.addToTree(data, 0, 0);
		}
	});
}

/****************************************************/
function reduceStat(node,info)
{
	if(node.info == undefined)
		node.info = jQuery.extend(true, {}, info);
	else
		maltHelper.mergeStackInfoDatas(node.info,info);
}

/****************************************************/
MaltCallStacksView.prototype.buildCallTree = function(data)
{
	var tree = {childs:{},id:null};
	var id = 0;
	console.log(data);
	data.forEach(function(call) {
		var cur = tree;
		reduceStat(cur,call.info);
		call.stack.reverse().forEach(function(loc) {
			loc.function = loc.function.replace('<', '&lt;').replace('>', '&gt;');
			if (cur.childs[loc.function] == undefined)
				cur.childs[loc.function] = {childs:{},id:id++,location:loc};
			cur = cur.childs[loc.function];
			reduceStat(cur,call.info);
		});
	});
	
	//alert(JSON.stringify(tree,null,'\t'));
	return tree;
}

/****************************************************/
MaltCallStacksView.prototype.onClick = function(location,infos)
{
	console.log("Click on call stack element");
	console.log(location);
	console.log(infos);
	//alert('click on '+JSON.stringify(location));
}

/****************************************************/
//add info to tree
MaltCallStacksView.prototype.addToTree = function(entries, parentId, parentLevel)
{
	var cur = this;
	//var metric = this.selector.getMetric();

	for (var i in entries)
	{
		//extract value
		var value = this.selector.getValue({total:entries[i].infos,own:entries[i].infos});
// 		var value = metric.extractor(treeNode.childs[i].info);
		if (value == 0)
			continue;
		//value = maltHelper.humanReadableValue(value,metric.unit);
		value = this.selector.getFormattedValue({total:entries[i].infos,own:entries[i].infos});

		//ids
		var id = `${entries[i].stackDepth}-${entries[i].stackId}`;
		var parentId = `${entries[i].parentStackDepth}-${entries[i].parentStackId}`;
		
		//create html
		var rows = $("<tr/>").attr('data-tt-id', id);
		if (entries[i].parentStackDepth != 0)
			rows = rows.attr('data-tt-parent-id', parentId);
		
		//mark for expand
		if (entries[i].hasChild)
			rows = rows.attr('data-tt-branch', 'true');

		var a = $('<span>'+entries[i].location.function+'</span>').click(entries[i],function(event) {
			cur.onClick(event.data.location,{total:event.data.infos,own:event.data.infos});
		})
			.css('cursor','pointer');
		var td = $('<td/>').append(a);
		rows.append(td);
		rows.append('<td id='+this.tableId+'.value.'+id+'>'+value+'</td>');
		
		//attach to parent
		var parentNode = null;
		if (entries[i].parentStackDepth != 0)
			parentNode = this.container.treetable('node', parentId);
		
		//insert
		this.container.treetable("loadBranch", parentNode, rows);
		
		//map user data
		var branch = this.container.treetable('node',id);
		branch.maltData = entries[i];
		
		//childs
		//this.addToTree(treeNode.childs[i],selector,expandedDepth-1);
		
		//post actions
		this.container.treetable("collapseNode", id);
		if (parentNode == null)
			this.stackViewRoots.push(id);
	}
}
