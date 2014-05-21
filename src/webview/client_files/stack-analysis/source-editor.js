/****************************************************/
function mergeStackMinMaxInfo(onto,value)
{
	onto.count += value.count;
	onto.sum += value.sum;
	if (onto.min == 0 || (value.min < onto.min && value.min != 0))
		onto.min = value.min;
	if (onto.max == 0 || (value.max > onto.max && value.max != 0))
		onto.max = value.max;
}

/****************************************************/
function mergeStackInfoDatas(onto,value)
{
	onto.countZeros += value.countZeros;
	onto.maxAliveReq += value.maxAliveReq;
	onto.aliveReq += value.aliveReq;
	mergeStackMinMaxInfo(onto.alloc,value.alloc);
	mergeStackMinMaxInfo(onto.free,value.free);
	mergeStackMinMaxInfo(onto.lifetime,value.lifetime);
}

/****************************************************/
function computeTotal(value)
{
	//already done
	if (value.total != undefined)
	{
		return;
	} else if (value.own == undefined) {
		value.total = jQuery.extend(true, {}, value.childs);
	} else {
		//copy
		value.total = jQuery.extend(true, {}, value.own);
		//merge
		if (value.childs != undefined)
			mergeStackInfoDatas(value.total,value.childs);
	}
}

/****************************************************/
function reduceStat(node,info)
{
	if(node.info == undefined)
		node.info = jQuery.extend(true, {}, info);
	else
		mergeStackInfoDatas(node.info,info);
}

/****************************************************/
function buildCallTree(data)
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
	return tree;
}

/****************************************************/
function MattFuncTree(tableId)
{
	//vars
	this.tableId = tableId;
	this.table = $('#'+tableId);
	
	//setup
	this.table.treetable({ expandable: true,initialState:'collasped',clickableNodeNames:false});
	this.stackViewRoots = [];
	this.tree = null;
	this.selector = null;
	
	//update content
	this.updateData = function(data)
	{
		//build tree
		var tree = buildCallTree(data);
		addToTree(tree);
	}
	
	this.onClick = function(location,infos)
	{
		alert('click on '+JSON.stringify(location));
	}
	
	//add info to tree
	this.addToTree = function(treeNode,selector,expandedDepth)
	{
		var cur = this;
		if (expandedDepth < -5)
			return;

		for (var i in treeNode.childs)
		{
			//extract value
			var value = selector.extractor(treeNode.childs[i].info);
			value = mattHumanValue({unit:selector.unit},value)
			
			//create html
			var rows = $("<tr/>").attr('data-tt-id',treeNode.childs[i].id);
			if (treeNode.id != null)
				rows = rows.attr('data-tt-parent-id',treeNode.id);

			var a = $('<span>'+i+'</span>').click(treeNode.childs[i],function(event) {
				cur.onClick(event.data.location,event.data.infos);
			})
				.css('cursor','pointer');
			var td = $('<td/>').append(a);
			rows.append(td);
			rows.append('<td id='+this.tableId+'.value.'+treeNode.childs[i].id+'>'+value+'</td>');
			
			//attach to parent
			var parentNode = null;
			if (treeNode.id != null)
				parentNode = this.table.treetable('node',treeNode.id);	
			
			//insert
			this.table.treetable("loadBranch", parentNode, rows);
			
			//childs
			this.addToTree(treeNode.childs[i],selector,expandedDepth-1);
			
			//post actions
			if (expandedDepth < 0)
				this.table.treetable("collapseNode", treeNode.childs[i].id);
			if (parentNode == null)
				this.stackViewRoots.push(treeNode.childs[i].id);
		}
	}
	
	this.countUntilDepth = function(tree,maxDepth)
	{
		//trivial
		if (maxDepth < 0)
			return 0;
		
		var cnt = 0;
		for(var i in tree.childs)
			cnt+=this.countUntilDepth(tree.childs[i],maxDepth-1)+1;

		return cnt;
	}
	
	//check max expended depth
	this.calcMaxEpendedDepth = function(tree,maxOpen)
	{
		var maxDepth = 0;
		while (maxDepth < 20 && this.countUntilDepth(tree,maxDepth) < maxOpen)
			maxDepth++;
		return maxDepth-1;
	}
	
	//clear
	this.clear = function()
	{
		var cur = this;
		this.stackViewRoots.forEach(function(value){
			cur.table.treetable('removeNode',value);
		});
		this.stackViewRoots = [];
	}
	
	this.update = function(file,line,selector)
	{
		var cur = this;
		this.clear();
		$.getJSON("/stacks.json?file="+encodeURIComponent(file)+"&line="+line,function(data) {
			var tree = buildCallTree(data);
			cur.addToTree(tree,selector,cur.calcMaxEpendedDepth(tree,8));
			cur.tree = tree;
		});
	}
	
	this.updateFunc = function(func,selector)
	{
		var cur = this;
		this.clear();
		$.getJSON("/stacks.json?func="+encodeURIComponent(func),function(data) {
			var tree = buildCallTree(data);
			cur.addToTree(tree,selector,cur.calcMaxEpendedDepth(tree,4));
			cur.tree = tree;
		});
	}
	
	this.updateSelectorInternal = function(tree,selector)
	{
		for(var i in tree.childs)
		{
			//extract value
			var value = selector.extractor(tree.childs[i].info);
			value = mattHumanValue({unit:selector.unit},value)
			
			//update value
			document.getElementById(this.tableId+'.value.'+tree.childs[i].id).innerHTML = value;
			
			//cuilds
			this.updateSelectorInternal(tree.childs[i],selector);
		}
	}
	
	this.updateSelector = function(selector)
	{
		if (this.tree != null)
			this.updateSelectorInternal(this.tree,selector);
	}
}

function MattEditor(divId)
{
	//store divid
	this.divId = divId;
	this.div = document.getElementById(divId);
	this.div.matt = this;
	var cur = this;

	//create code mirror
	this.editor = CodeMirror(this.div,{
		value: "//function myScript(){return 100;}\n",
		mode:  "clike",
		lineNumbers: true,
		//theme:'lesser-dark',
		theme:'eclipse',
		indentWithTabs:true,
		fixedGutter:true,
		readOnly:true,
		styleActiveLine: true,
		gutters: ["matt-annotations","CodeMirror-linenumbers"]
	});
	
// 	$("#matt-alloc-stacks-tree").treetable({ expandable: true,initialState:'collasped',clickableNodeNames:true});
// 	$("#matt-alloc-stacks-tree")[0].stackViewRoots = [];
	//tree
	this.funcTree = new MattFuncTree("matt-alloc-stacks-tree");
	this.funcTree.onClick = function(location,infos)
	{
		if (location.file == "??" || location.line == -1)
			alert("Unknown location : "+location.file + " " + location.line);
		cur.updateFile(location.file,location.line,false);
	}
	
	//cur file name
	this.file = null;
	this.fileStats = null;
	this.selector = null;
	this.mode = null;

	//set config
	this.setConfig = function(selector,mode)
	{
		this.selector = selector;
		this.mode = mode;
	}
	
	this.update = function(details,force)
	{
		if(details.file != '??' && details.file != '' && details.file != undefined)
		{
			this.updateFile(details.file,details.line,true);
		} else if (details.function != undefined && details.function != '??' && details.function != '') {
			this.updateFunc(details.function,details);
		}
		
		if (details.function != undefined && details.function != '??' && details.function != '') {
			this.updateDetails(details.function,details);
		}
	}
	
	this.updateDetails = function(func,info)
	{
		var callback = new EJS({url: '/stack-analysis/alloc-site-details.ejs'}).update("matt-alloc-info");
		callback({info:info});
		this.funcTree.updateFunc(func,this.selector);
	}
	
	this.updateFunc = function(func,info)
	{
		this.editor.setValue("//Uknown source file, maybe debug informations are missing !\n");
		this.editor.setCursor(1);
		this.file = null;
	}

	//Function to update file in editor
	this.updateFile = function(file,line,force)
	{
		///trivial
		if (file == this.file && force != true)
		{
			this.editor.setCursor(line);
			return;
		}
	
		//check if need to load file or if unknown
		if(file == '??' || file == '' || file == undefined)
		{
			this.editor.setValue("//Uknown source file, maybe debug informations are missing !\n");
			this.editor.setCursor(1);
			this.file = null;
		} else {
			var mattObj = this;
			$.get( "/app-sources"+file,function(data){
				mattObj.editor.setValue(data);
				mattObj.editor.setCursor(line);
				mattObj.file = file;
				mattObj.updateAnotations(file);
			});
		}
	}
	
	function dataToTextDetails(data)
	{
		return JSON.stringify(data,null,"\t");
	}
	
	this.makeMarker = function(selector,mode,data,max) {
		if (data[mode] == undefined)
			return null;
		
		var value = selector.extractor(data[mode]);
		if (value == undefined || value == 0)
			return null;
		
		var marker = document.createElement("div");
		if (value < max / 3.0)
			marker.className = 'matt-annotation-small';
		else if (value < 2.0 * max / 3.0 )
			marker.className = 'matt-annotation-medium';
		else
			marker.className = 'matt-annotation-large';
		marker.innerHTML = mattHumanValue({unit:selector.unit},value);
		marker.mattData = data;
		var cur = this;
		marker.onclick = function() {
			var callback = new EJS({url: '/stack-analysis/alloc-site-details.ejs'}).update("matt-alloc-info");
			callback({info:this.mattData});
			cur.funcTree.update(this.mattData.file,this.mattData.line,selector);
		};
		return marker;
	}
	
	//extract max
	this.extractMax = function(selector,mode,data)
	{
		var max = 0;
		for (var i in data)
		{
			if (data[i][mode] != undefined)
			{
				var value = selector.extractor(data[i][mode]);
				if (value != undefined && value > max)
					max = value;
			}
		}
		return max;
	}
	
	//update anotations
	this.updateAnotations = function(file)
	{
		var cur = this;
		$.getJSON("/file-infos.json?file="+file,function(data) {
			cur.data = data;
			for (var i in data)
			{
				data[i].file = cur.file;
				computeTotal(data[i]);
			}
			
			var max = cur.extractMax(cur.selector,cur.mode,data);
			
			for (var i in data)
			{
				cur.editor.setGutterMarker(data[i].line-1, "matt-annotations",cur.makeMarker(cur.selector,cur.mode,data[i],max));
			}
		});
	}
	
	this.redrawAnnotations = function()
	{
		this.editor.clearGutter
		var max = this.extractMax(this.selector,this.mode,this.data);
		for (var i in this.data)
			this.editor.setGutterMarker(this.data[i].line-1, "matt-annotations",this.makeMarker(this.selector,this.mode,this.data[i],max));
		this.funcTree.updateSelector(this.selector);
	}
}
