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
	this.syntaxHighlighterEle = null;
	
	//check and link
	//maltHelper.assert(this.container != undefined);
	this.container.malt = this;
	
	//defaults
	this.file = null;
	this.data = null;
	this.postMove = null;
	this.selector = selector;
}

/********************************************************************/
//To be override by user to capture events
MaltSourceEditor.prototype.onClick = function(infos)
{
	alert(JSON.stringify(infos,null,'\t'));
}

/********************************************************************/
MaltSourceEditor.prototype.doPostMove = function()
{
	if (this.postMove.type == 'line' && this.postMove.line != -1)
	{
		// this.editor.setCursor(this.postMove.line-1);
		Prism.plugins.codeAnnotator.scrollToAndHighlight(
			this.syntaxHighlighterEle, this.postMove.line);
	} else if (this.postMove.type == 'func') {
		var line = this.findLargestAnnot(this.file,this.postMove.func);
		if (line != -1)
			Prism.plugins.codeAnnotator.scrollToAndHighlight(
				this.syntaxHighlighterEle, line);

	}
}

/********************************************************************/
MaltSourceEditor.prototype.getLanguage = function(filename)
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
			return "Fortran";
		case 'c':
		case 'h':
		case 'cxx':
		case 'cpp':
		case 'hxx':
		case 'h++':
		case 'hpp':
		case 'ainsic':
			return "C++";
		case 'py':
			return "Python";
		default:
			return "C++";
	}
}

/********************************************************************/
MaltSourceEditor.prototype.getLanguageClassForHighlighter = function(name) {
	name = name.toLowerCase();
	switch(name) 
	{
		case 'fortan':
			return 'language-fortran';
		case 'c':
		case 'c++':
			return 'language-cpp';
		case 'python':
			return 'language-python';
		default:
			return '';
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
	if(file == '??' || file == '' || file == undefined) {
		return;
	} else {
		var cur = this;
		maltDataSource.loadSourceFile(file,function(data){
			// File loaded, now highlight it
			cur.container.innerHTML = 
				'<pre class="line-numbers"><code class="' + 
				cur.getLanguageClassForHighlighter(cur.getLanguage(file)) + '">' +
				safe_tags_replace(data) + '</code></pre>';
			cur.syntaxHighlighterEle = cur.container.getElementsByTagName("code")[0];
			Prism.highlightElement(cur.syntaxHighlighterEle);
			cur.file = file;
			cur.updateAnotations();
		}, function() {
			// XHR fails to load file, show error message
			cur.container.innerHTML = 
				'<pre class="line-numbers"><code>Source for the file, ' 
				+ file + ', could not be loaded.</code></pre>';
			cur.syntaxHighlighterEle = cur.container.getElementsByTagName("code")[0];
			Prism.highlightElement(cur.syntaxHighlighterEle);
			Prism.plugins.codeAnnotator.add(cur.syntaxHighlighterEle, {
				line: 1, 
				text: "Error", 
				class: "line-annotate-large"
			});
		});
	}
}

/********************************************************************/
MaltSourceEditor.prototype.findLargestAnnot = function(file,func)
{
	var line = -1;
	var max = 0;

	for (var i in this.data)
	{
		//var value = this.data[i][mode];
		var value = this.selector.getValue(this.data[i]);

		if (value != undefined && this.data[i].file == file && this.data[i].function == func)
		{
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
MaltSourceEditor.prototype.extractMax = function(data)
{
	//setup some vars
	var max = 0;

	//loop on all datas
	for (var i in data)
	{
		var value = this.selector.getValue(data[i]);
		if (value != undefined)
		{
			if (value != undefined && value > max)
				max = value;
		}
	}
	
	//return max
	return max;
}

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
	//search max to compute color gradiant
	var max = this.extractMax(this.data);
	
	//use D3JS for color gradiant from blue -> red
	var colorScale = d3.scale.linear()
		.range(["#397EBA","#FF9595"])
		.domain([0,max]);
	
	//clear
	Prism.plugins.codeAnnotator.removeAll(this.syntaxHighlighterEle);

	//insert all markers
	var cur = this;
	for (var i in this.data) {
		if (this.selector.getValue(this.data[i]) != 0)
		Prism.plugins.codeAnnotator.add(this.syntaxHighlighterEle, {
			line: this.data[i].line, 
			text: this.selector.getFormattedValue(this.data[i]), 
			// class: "line-annotate-small",
			color: colorScale(this.selector.getValue(this.data[i])),
			onClick: function(ele, data) {
				cur.onClick(data.data)
			},
			data: this.data[i],
		});
	}
}
