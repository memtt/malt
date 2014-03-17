function MattEditor(divId)
{
	//store divid
	this.divId = divId;
	this.div = document.getElementById(divId);
	this.div.matt = this;

	//create code mirror
	this.editor = CodeMirror(this.div,{
		value: "//function myScript(){return 100;}\n",
		mode:  "clike",
		lineNumbers: true,
		theme:'lesser-dark',
		indentWithTabs:true,
		fixedGutter:true,
		readOnly:true,
		styleActiveLine: true,
		gutters: ["matt-annotations","CodeMirror-linenumbers"]
	});
	
	$("#matt-alloc-stacks-tree").treetable({ expandable: true,initialState:'expended',clickableNodeNames:true});
	
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
	
	function addToTree(data)
	{
		var table = $("#matt-alloc-stacks-tree");
		for (var i in data.childs)
		{
			var rows = $("<tr/>").attr('data-tt-id',data.childs[i].id);
			if (data.id != null)
				rows = rows.attr('data-tt-parent-id',data.id);
			rows.append('<td>'+i+'</td>');
			alert(data.id + " => " + i + " => " + data.childs[i].id);
			var parentNode = null;
			if (data.id != null)
				parentNode = table.treetable('node',data.id);	
			table.treetable("loadBranch", parentNode, rows);
			alert("OK : " +data.id + " | " + parentNode + " => " + i + " => " + data.childs[i].id);
			addToTree(data.childs[i]);
		}
	}
	
	function makeMarker(selector,mode,data) {
		if (data[mode] == undefined)
			return null;
		
		var value = selector.extractor(data[mode]);
		if (value == undefined || value == 0)
			return null;
		
		var marker = document.createElement("div");
		marker.className = 'matt-annotation';
		marker.innerHTML = mattHumanValue({unit:selector.unit},value);
		marker.mattData = data;
		marker.onclick = function() {
			var callback = new EJS({url: '/stack-analysis/alloc-site-details.ejs'}).update("matt-alloc-info");
			callback({info:this.mattData});
			$.getJSON("/stacks.json?file="+this.mattData.file+"&line="+this.mattData.line,function(data) {
				var tree = buildCallTree(data);
				addToTree(tree);
			});
		};
		return marker;
	}
	
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
	
	function reduceStat(node,info)
	{
		if(node.info == undefined)
			node.info = jQuery.extend(true, {}, info);
		else
			mergeStackInfoDatas(node.info,info);
	}
	
	function buildCallTree(data)
	{
		var tree = {childs:{},id:null};
		var id = 0;
		data.forEach(function(call) {
			var cur = tree;
			reduceStat(cur,call.info);
			call.stack.reverse().forEach(function(loc) {
				if (cur.childs[loc.function] == undefined)
					cur.childs[loc.function] = {childs:{},id:id++};
				cur = cur.childs[loc.function];
				reduceStat(cur,call.info);
			});
		});
		alert(JSON.stringify(tree,null,'\t'));
		return tree;
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
				cur.editor.setGutterMarker(data[i].line-1, "matt-annotations",makeMarker(cur.selector,cur.mode,data[i]));
			}
		});
	}
	
	this.redrawAnnotations = function()
	{
		this.editor.clearGutter
		for (var i in this.data)
			this.editor.setGutterMarker(this.data[i].line-1, "matt-annotations",makeMarker(this.selector,this.mode,this.data[i]));
	}
}
