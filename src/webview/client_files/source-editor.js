/********************************************************************/
/**
 * Provide a source code editor to annotate sources with profiled values.
 * @param containerId Provide the ID of the div in which to setup the code editor.
 * @param selector Define a metric selector to extract call sites infos for annotations
**/
function MattSourceEditor(containerId,selector)
{
	//errors
	mattHelper.assert(selector != undefined && selector instanceof MattFuncListSelector);
	
	//setup container
	this.containerId = containerId;
	this.container = document.getElementById(containerId);
	
	//check and link
	mattHelper.assert(this.container != undefined);
	this.container.matt = this;
	
	//defaults
	this.file = null;
	this.data = null;
	this.postMove = null;
	this.selector = selector;

	//make rendering
	this.initRentering();
}

/********************************************************************/
//To be override by user to capture events
MattSourceEditor.prototype.onClick = function(infos)
{
	alert(JSON.stringify(infos,null,'\t'));
}

/********************************************************************/
MattSourceEditor.prototype.initRentering = function()
{
	//create code mirror
	this.editor = CodeMirror(this.container,{
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
}

/********************************************************************/
MattSourceEditor.prototype.doPostMove = function()
{
	if (this.postMove.type == 'line' && this.postMove.line != -1)
	{
		this.editor.setCursor(this.postMove.line-1);
	} else if (this.postMove.type == 'func') {
		var line = this.findLargestAnnot(this.file,this.postMove.func);
		if (line != -1)
			this.editor.setCursor(line-1);
	}
}

/********************************************************************/
MattSourceEditor.prototype.moveToFile = function(file)
{
	//nothing to do
	if (this.file == file)
	{
		this.doPostMove();
		return;
	}
	
	//load the new file in editor
	if(file == '??' || file == '' || file == undefined)
	{
		this.editor.setValue("//Uknown source file, maybe debug informations are missing !\n");
		this.editor.setCursor(1);
		this.file = null;
	} else {
		var cur = this;
		$.get( "/app-sources"+file,function(data){
			
			//set mode
			var ext = (/[.]/.exec(file)) ? /[^.]+$/.exec(file) : undefined;

			alert(ext);
			/*if (ext != undefined)
				ext = ext.toLowerCase();*/

			if (ext == undefined)
				cur.editor.setOption("mode","clike");
			else if (ext == 'c' || ext == 'h' || ext == 'cxx' || ext == 'c++' || ext == 'cpp' || ext == 'hxx' || ext == 'h++' || ext == 'hpp')
				cur.editor.setOption("mode","clike");
			else if (ext == 'f' || ext == 'f77' || ext == 'f90')
				cur.editor.setOption("mode","fortran");
			else
				cur.editor.setOption("mode","clike");
			
			cur.editor.setValue(data);
			cur.editor.setCursor(1);
			cur.file = file;
			cur.updateAnotations();
		});
	}
}

/********************************************************************/
MattSourceEditor.prototype.moveToFileLine = function(file,line)
{
	if (line != -1)
		this.postMove = {type:'line',line:line};
	else
		this.postMove = {};
	this.moveToFile(file);
}

/********************************************************************/
MattSourceEditor.prototype.findLargestAnnot = function(file,func)
{
	var line = -1;
	var metric = this.selector.metric;
	var mode = this.selector.mode;
	var max = 0;

	for (var i in this.data)
	{
		var value = this.data[i][mode];

		if (value != undefined && this.data[i].file == file && this.data[i].function == func)
		{
			value = metric.extractor(value);
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
MattSourceEditor.prototype.moveToFileFunction = function(file,func)
{
	if (func != -1 && func != '' && func != '??')
		this.postMove = {type:'func',func:func};
	else
		this.postMove = {};
	this.moveToFile(file);
}

/****************************************************/
MattSourceEditor.prototype.internalMergeStackMinMaxInfo = function(onto,value)
{
	onto.count += value.count;
	onto.sum += value.sum;
	if (onto.min == 0 || (value.min < onto.min && value.min != 0))
		onto.min = value.min;
	if (onto.max == 0 || (value.max > onto.max && value.max != 0))
		onto.max = value.max;
}

/****************************************************/
MattSourceEditor.prototype.internalMergeStackInfoDatas = function(onto,value)
{
	onto.countZeros += value.countZeros;
	onto.maxAliveReq += value.maxAliveReq;
	onto.aliveReq += value.aliveReq;
	this.internalMergeStackMinMaxInfo(onto.alloc,value.alloc);
	this.internalMergeStackMinMaxInfo(onto.free,value.free);
	this.internalMergeStackMinMaxInfo(onto.lifetime,value.lifetime);
}

/********************************************************************/
MattSourceEditor.prototype.internalComputeTotal = function(value)
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
MattSourceEditor.prototype.extractMax = function(data)
{
	//setup some vars
	var max = 0;
	var mode = this.selector.mode;
	var metric = this.selector.getMetric();

	//loop on all datas
	for (var i in data)
	{
		var value = data[i][mode];
		if (value != undefined)
		{
			var value = metric.extractor(value);
			if (value != undefined && value > max)
				max = value;
		}
	}
	
	//return max
	return max;
}

/********************************************************************/
/**
 * Build a marker HTML DOM do be insert as annotations into codemirror
**/
MattSourceEditor.prototype.makeMarker = function(data,max,colorScale) {

	//some local vars
	var mode = this.selector.mode;
	var metric = this.selector.getMetric();
	
	//no data
	if (data[mode] == undefined)
		return null;
		
	//extract value
	var value = metric.extractor(data[mode]);
	if (value == undefined || value == 0)
		return null;

	//create marker DIV
	var marker = document.createElement("div");
	
	//select basic 3 colors (TO REMOVE)
	if (value < max / 3.0)
		marker.className = 'matt-annotation-small';
	else if (value < 2.0 * max / 3.0 )
		marker.className = 'matt-annotation-medium';
	else
		marker.className = 'matt-annotation-large';
	
	//apply color gradiant
	marker.style = "background-color:"+colorScale(value);
	
	//setup values
	marker.innerHTML = this.mattHumanValue(value,metric.unit);
	marker.mattData = data;
	
	//manage onclick
	var cur = this;
	marker.onclick = function() {
		cur.onClick(this.mattData);
	};
	
	//ok return
	return marker;
}

/********************************************************************/
var MATT_POWER = ['&nbsp;','K','M','G','T','P'];
MattSourceEditor.prototype.mattHumanValue = function(value,unit)
{
	var power = 0;
	while (value >= 1024)
	{
		power++;
		value /= 1024;
	}

	//return value.toFixed(1) + " " + MATT_POWER[power] + data.unit;
	return Math.round(value) + " " + MATT_POWER[power] + unit;
}

/********************************************************************/
//update anotations
MattSourceEditor.prototype.updateAnotations = function()
{
	//keep track of current this
	var cur = this;
	var file = this.file;
	
	//fetch flat profile of current file
	$.getJSON("/file-infos.json?file="+file,function(data) {

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
MattSourceEditor.prototype.redrawAnnotations = function()
{
	//clear old
	this.editor.clearGutter();
	
	//search max to compute color gradiant
	var max = this.extractMax(this.data);
	
	//use D3JS for color gradiant from blue -> red
	var colorScale = d3.scale.linear()
		.range(["#397EBA","#FF9595"])
		.domain([0,max]);
	
	//insert all markers
	for (var i in this.data)
		this.editor.setGutterMarker(this.data[i].line-1, "matt-annotations",this.makeMarker(this.data[i],max,colorScale));
}
