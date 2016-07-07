/********************************************************************/
/**
 * Provide a source code editor to annotate sources with profiled values.
 * @param containerId Provide the ID of the div in which to setup the code editor.
 * @param selector Define a metric selector to extract call sites infos for annotations
**/
function MaltSourceEditor(containerId,selector)
{
	//errors
	//maltHelper.assert(selector != undefined && selector instanceof MaltSelector);
	
	//setup container
	this.containerId = containerId;
	this.container = document.getElementById(containerId);
	
	//check and link
	//maltHelper.assert(this.container != undefined);
	this.container.malt = this;
	
	//defaults
	this.file = null;
	this.data = null;
	this.postMove = null;
	this.selector = selector;

	//make rendering
	// this.initRentering();
}

/********************************************************************/
//To be override by user to capture events
MaltSourceEditor.prototype.onClick = function(infos)
{
	alert(JSON.stringify(infos,null,'\t'));
}

/********************************************************************/
// MaltSourceEditor.prototype.initRentering = function()
// {
// 	//create code mirror
// 	this.editor = CodeMirror(this.container,{
// 		value: "//Failed to load file\n",
// 		mode:  "clike",
// 		lineNumbers: true,
// 		//theme:'lesser-dark',
// 		theme:'eclipse',
// 		indentWithTabs:true,
// 		fixedGutter:true,
// 		readOnly:true,
// 		styleActiveLine: true,
// 		gutters: ["malt-annotations","CodeMirror-linenumbers"]
// 	});
// }

/********************************************************************/
MaltSourceEditor.prototype.doPostMove = function()
{
	if (this.postMove.type == 'line' && this.postMove.line != -1)
	{
		// this.editor.setCursor(this.postMove.line-1);
		Prism.plugins.codeAnnotator.scrollToAndHighlight(
			document.getElementById("malt-source-editor-box"), this.postMove.line);
	} else if (this.postMove.type == 'func') {
		var line = this.findLargestAnnot(this.file,this.postMove.func);
		if (line != -1)
			Prism.plugins.codeAnnotator.scrollToAndHighlight(
				document.getElementById("malt-source-editor-box"), line);

	}
}

/********************************************************************/
MaltSourceEditor.prototype.getColorationType = function(filename)
{
	//set mode
	var ext = (/[.]/.exec(filename)) ? /[^.]+$/.exec(filename) : undefined;
	if (ext != undefined)
		ext = (ext+"").toLowerCase();

	switch(ext)
	{
		case 'f':
		case 'f90':
		case 'f77':
			return "fortran";
		case 'c':
		case 'h':
		case 'cxx':
		case 'cpp':
		case 'hxx':
		case 'h++':
		case 'hpp':
		case 'ainsic':
			return "clike";
		default:
			return "clike";
	}
}

/********************************************************************/
MaltSourceEditor.prototype.moveToFile = function(file)
{
	//nothing to do
	if (this.file == file)
	{
		this.doPostMove();
		return;
	}

	var tagsToReplace = {
	    '&': '&amp;',
	    '<': '&lt;',
	    '>': '&gt;'
	};

	function replaceTag(tag) {
	    return tagsToReplace[tag] || tag;
	}

	function safe_tags_replace(str) {
	    return str.replace(/[&<>]/g, replaceTag);
	}
	
	//load the new file in editor
	if(file == '??' || file == '' || file == undefined)
	{
		this.file = file;
		alert("No source file found.");
	} else {
		var cur = this;
		maltDataSource.loadSourceFile(file,function(data){
			document.getElementById("malt-source-editor").innerHTML = '<pre class="line-numbers"><code id="malt-source-editor-box" class="language-cpp">' +
				safe_tags_replace(data) + '</code></pre>';
			cur.syntaxHighlighterEle = document.getElementById("malt-source-editor-box");
			Prism.highlightElement(cur.syntaxHighlighterEle);
			cur.file = file;
			cur.updateAnotations();
		});
	}
}

/********************************************************************/
// MaltSourceEditor.prototype.moveToFileLine = function(file,line)
// {
// 	if (line != -1)
// 		this.postMove = {type:'line',line:line};
// 	else
// 		this.postMove = {};
// 	this.moveToFile(file);
// }

/********************************************************************/
MaltSourceEditor.prototype.findLargestAnnot = function(file,func)
{
	var line = -1;
// 	var metric = this.selector.metric;
// 	var mode = this.selector.mode;
	var max = 0;

	for (var i in this.data)
	{
		//var value = this.data[i][mode];
		var value = this.selector.getValue(this.data[i]);

		if (value != undefined && this.data[i].file == file && this.data[i].function == func)
		{
			//value = metric.extractor(value);
			if (value > max)
			{
				max = value;
				line = this.data[i].line;
			}
		}
	}

	return line;
}

/********************************************************************/
MaltSourceEditor.prototype.moveToFileFunction = function(file,func)
{
	if (func != -1 && func != '' && func != '??')
		this.postMove = {type:'func',func:func};
	else
		this.postMove = {};
	this.moveToFile(file);
}

/****************************************************/
MaltSourceEditor.prototype.internalMergeStackMinMaxInfo = function(onto,value)
{
	onto.count += value.count;
	onto.sum += value.sum;
	if (onto.min == 0 || (value.min < onto.min && value.min != 0))
		onto.min = value.min;
	if (onto.max == 0 || (value.max > onto.max && value.max != 0))
		onto.max = value.max;
}

/****************************************************/
MaltSourceEditor.prototype.internalMergeStackInfoDatas = function(onto,value)
{
	onto.countZeros += value.countZeros;
	onto.maxAliveReq += value.maxAliveReq;
	onto.aliveReq += value.aliveReq;
	this.internalMergeStackMinMaxInfo(onto.alloc,value.alloc);
	this.internalMergeStackMinMaxInfo(onto.free,value.free);
	this.internalMergeStackMinMaxInfo(onto.lifetime,value.lifetime);
}

/********************************************************************/
MaltSourceEditor.prototype.internalComputeTotal = function(value)
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
			this.internalMergeStackInfoDatas(value.total,value.childs);
	}
}

/********************************************************************/
//extract max
// MaltSourceEditor.prototype.extractMax = function(data)
// {
// 	//setup some vars
// 	var max = 0;
// // 	var mode = this.selector.mode;
// // 	var metric = this.selector.getMetric();

// 	//loop on all datas
// 	for (var i in data)
// 	{
// 		//var value = data[i][mode];
// 		var value = this.selector.getValue(data[i]);
// 		if (value != undefined)
// 		{
// 			//var value = metric.extractor(value);
// 			if (value != undefined && value > max)
// 				max = value;
// 		}
// 	}
	
// 	//return max
// 	return max;
// }

/********************************************************************/
/**
 * Build a marker HTML DOM do be insert as annotations into codemirror
**/
// MaltSourceEditor.prototype.makeMarker = function(data,max,colorScale) {

// 	//some local vars
// // 	var mode = this.selector.mode;
// // 	var metric = this.selector.getMetric();
	
// 	//no data
// // 	if (data[mode] == undefined)
// // 		return null;
		
// 	//extract value
// 	//var value = metric.extractor(data[mode]);
// 	var value = this.selector.getValue(data);
// 	if (value == undefined || value == 0)
// 		return null;

// 	//create marker DIV
// 	var marker = document.createElement("div");
	
// 	//select basic 3 colors (TO REMOVE)
// 	if (value < max / 3.0)
// 		marker.className = 'malt-annotation-small';
// 	else if (value < 2.0 * max / 3.0 )
// 		marker.className = 'malt-annotation-medium';
// 	else
// 		marker.className = 'malt-annotation-large';
	
// 	//apply color gradiant
// 	marker.style = "background-color:"+colorScale(value);
	
// 	//setup values
// 	//marker.innerHTML = this.maltHumanValue(value,metric.unit);
// 	marker.innerHTML = this.selector.getFormattedValue(data);
// 	marker.maltData = data;
	
// 	//manage onclick
// 	var cur = this;
// 	marker.onclick = function() {
// 		cur.onClick(this.maltData);
// 	};
	
// 	//ok return
// 	return marker;
// }

/********************************************************************/
// var MALT_POWER = ['&nbsp;','K','M','G','T','P'];
// MaltSourceEditor.prototype.maltHumanValue = function(value,unit)
// {
// 	var power = 0;
// 	while (value >= 1024)
// 	{
// 		power++;
// 		value /= 1024;
// 	}

// 	//return value.toFixed(1) + " " + MALT_POWER[power] + data.unit;
// 	return Math.round(value) + " " + MALT_POWER[power] + unit;
// }

/********************************************************************/
//update anotations
MaltSourceEditor.prototype.updateAnotations = function()
{
	//keep track of current this
	var cur = this;
	var file = this.file;
	
	//fetch flat profile of current file
	maltDataSource.loadSourceFileAnnotations(file,function(data) {

		//update data with more info than provided by server
		cur.data = data;
		for (var i in data)
		{
			data[i].file = cur.file;
			cur.internalComputeTotal(data[i]);
		}
		
		//draw annotations
		cur.redrawAnnotations();
		
		//move
		cur.doPostMove();
	});
}

/********************************************************************/
MaltSourceEditor.prototype.redrawAnnotations = function()
{
	//clear old
	// this.editor.clearGutter();
	
	//search max to compute color gradiant
	// var max = this.extractMax(this.data);
	
	//use D3JS for color gradiant from blue -> red
	// var colorScale = d3.scale.linear()
	// 	.range(["#397EBA","#FF9595"])
	// 	.domain([0,max]);
	
	//insert all markers
	var cur = this;
	for (var i in this.data) {
		Prism.plugins.codeAnnotator.add(this.syntaxHighlighterEle, {
			line: this.data[i].line, 
			text: this.selector.getFormattedValue(this.data[i]), 
			class: "line-annotate-small",
			onClick: function(ele, data) {
				// console.log("bru bru bru");
				cur.onClick(data.data)
			},
			data: this.data[i]
		});
		// this.editor.setGutterMarker(this.data[i].line-1, "malt-annotations",this.makeMarker(this.data[i],max,colorScale));
	}
}
