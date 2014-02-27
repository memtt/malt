function MattEditor(divId)
{
	//store divid
	this.divId = divId;
	this.div = document.getElementById(divId);
	this.div.matt = this;
	
	//create editor
	this.editor = ace.edit(divId);
	this.editor.setTheme("ace/theme/tomorrow_night");
	this.editor.getSession().setMode("ace/mode/c_cpp");
	this.editor.setReadOnly(true);
	
	//cur file name
	this.file = null;
	this.fileStats = null;

	//Function to update file in editor
	this.updateFile = function(file,line)
	{
		///trivial
		if (file == this.file)
		{
			this.editor.gotoLine(line);
			return;
		}
	
		//check if need to load file or if unknown
		if(file == '??' || file == '' || file == undefined)
		{
			this.editor.setValue("//Uknown source file, maybe debug informations are missing !\n");
			this.editor.resize(true);
			this.editor.gotoLine(1);
			this.file = null;
		} else {
			var mattObj = this;
			$.get( "/app-sources"+file,function(data){
				mattObj.editor.setValue(data);
				mattObj.editor.resize(true);
				mattObj.editor.gotoLine(line);
				mattObj.file = file;
				mattObj.updateAnotations(file);
			});
		}
	}
	
	//update anotations
	this.updateAnotations = function(file)
	{
		var cur = this;
		$.getJSON("/file-infos.json?file="+file,function(data) {
			var annot = new Array();
			for (var i in data)
			{
				data[i].file = undefined;
				data[i].binary = undefined;
				annot.push({row: i, column: 0, html:'<pre>'+JSON.stringify(data[i],null,"\t")+'</pre>', type:"info"});
			}
			cur.editor.getSession().setAnnotations(annot);
		});
	}
}
