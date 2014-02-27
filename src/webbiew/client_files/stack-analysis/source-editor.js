function MattEditor(divId)
{
	//store divid
	this.divId = divId;
	this.div = document.getElementById(divId);
	this.div.matt = this;
	
	//create editor
	/*this.editor = ace.edit(divId);
	this.editor.setTheme("ace/theme/tomorrow_night");
	this.editor.getSession().setMode("ace/mode/c_cpp");
	this.editor.setReadOnly(true);
	var Range = ace.require('ace/range').Range;*/
	
	this.editor = CodeMirror(this.div,{
		value: "//function myScript(){return 100;}\n",
		mode:  "clike",
		lineNumbers: true,
		theme:'lesser-dark',
		indentWithTabs:true,
		fixedGutter:true,
		readOnly:true,
		styleActiveLine: true,
// 		viewportMargin:'Infinity',
// 		lineWrapping:true,
		gutters: ["matt-annotations","CodeMirror-linenumbers"]
	});
	
	//cur file name
	this.file = null;
	this.fileStats = null;

	//Function to update file in editor
	this.updateFile = function(file,line)
	{
		///trivial
		if (file == this.file)
		{
// 			this.editor.gotoLine(line);
			this.editor.setCursor(line);
			return;
		}
	
		//check if need to load file or if unknown
		if(file == '??' || file == '' || file == undefined)
		{
// 			this.editor.setValue("//Uknown source file, maybe debug informations are missing !\n");
// 			this.editor.resize(true);
// 			this.editor.gotoLine(1);
			this.editor.setValue("//Uknown source file, maybe debug informations are missing !\n");
			this.editor.setCursor(1);
			this.file = null;
		} else {
			var mattObj = this;
			$.get( "/app-sources"+file,function(data){
// 				mattObj.editor.setValue(data);
// 				mattObj.editor.resize(true);
// 				mattObj.editor.gotoLine(line);
				
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
	
	function makeMarker(data) {
		if (data.own == undefined || data.own.alloc == undefined)
			return null;
		
		var value = data.own.alloc.sum;
		if (value == undefined || value == 0)
			return null;
		
		var marker = document.createElement("div");
		marker.className = 'matt-annotation';
		marker.innerHTML = mattHumanValue({unit:'B'},value);
		marker.mattData = data;
		marker.onclick = function() {document.getElementById("matt-alloc-info").innerHTML = dataToTextDetails(this.mattData);};
		return marker;
	}
	
	//update anotations
	this.updateAnotations = function(file)
	{
		var cur = this;
		$.getJSON("/file-infos.json?file="+file,function(data) {
// 			var session = cur.editor.getSession();
			var annot = new Array();
			for (var i in data)
			{
				data[i].file = undefined;
				data[i].binary = undefined;
				annot.push({row: i-1, column: 0, html:'<pre>'+JSON.stringify(data[i],null,"\t")+'</pre>', type:"info"});
				
// 				var range = new Range(i, 1, i, 20);
// 				var markerId = session.addMarker(range,"matt_warning", "text",false);
				
				cur.editor.setGutterMarker(i-1, "matt-annotations",makeMarker(data[i]));
			}
// 			session.setAnnotations(annot);
// 			session.addMarker(range, "matt_info");
			
// 			setTimeout(function() {
// 			d3.selectAll(".ace_gutter .ace_gutter-cell").style('width','200px');},1000);
		});
	}
}
