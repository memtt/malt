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
			cur.addToTree(node.maltData);
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

	maltDataSource.getCallStackDataFileLine(file,line,function(data) {
		if (state == cur.asyncState)
		{
			var tree = cur.buildCallTree(data);
			cur.addToTree(tree);
			cur.tree = tree;
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

	maltDataSource.getCallStackDataFunc(func,function(data) {
		if (state == cur.asyncState)
		{
			var tree = cur.buildCallTree(data);
			cur.addToTree(tree);
			cur.tree = tree;

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
	data.forEach(function(call) {
		var cur = tree;
		reduceStat(cur,call.info);
		call.stack.reverse().forEach(function(loc) {
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
MaltCallStacksView.prototype.addToTree = function(treeNode)
{
	var cur = this;
	//var metric = this.selector.getMetric();

	for (var i in treeNode.childs)
	{
		//extract value
		var value = this.selector.getValue({total:treeNode.childs[i].info,own:treeNode.childs[i].info});
// 		var value = metric.extractor(treeNode.childs[i].info);
		if (value == 0)
			continue;
		//value = maltHelper.humanReadableValue(value,metric.unit);
		value = this.selector.getFormattedValue({total:treeNode.childs[i].info,own:treeNode.childs[i].info});
		
		//create html
		var rows = $("<tr/>").attr('data-tt-id',treeNode.childs[i].id);
		if (treeNode.id != null)
			rows = rows.attr('data-tt-parent-id',treeNode.id);
		
		//mark for expand
		if (treeNode.childs[i].childs != undefined && !jQuery.isEmptyObject(treeNode.childs[i].childs))
			rows = rows.attr('data-tt-branch', 'true');

		var a = $('<span>'+i+'</span>').click(treeNode.childs[i],function(event) {
			cur.onClick(event.data.location,{total:treeNode.childs[i].info,own:treeNode.childs[i].info});
		})
			.css('cursor','pointer');
		var td = $('<td/>').append(a);
		rows.append(td);
		rows.append('<td id='+this.tableId+'.value.'+treeNode.childs[i].id+'>'+value+'</td>');
		
		//attach to parent
		var parentNode = null;
		if (treeNode.id != null)
			parentNode = this.container.treetable('node',treeNode.id);	
		
		//insert
		this.container.treetable("loadBranch", parentNode, rows);
		
		//map user data
		var branch = this.container.treetable('node',treeNode.childs[i].id);
		branch.maltData = treeNode.childs[i];
		
		//childs
		//this.addToTree(treeNode.childs[i],selector,expandedDepth-1);
		
		//post actions
		this.container.treetable("collapseNode", treeNode.childs[i].id);
		if (parentNode == null)
			this.stackViewRoots.push(treeNode.childs[i].id);
	}
}
