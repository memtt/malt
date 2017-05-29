declare var d3: any;
declare var saveAs: any;

import { Component, OnInit } from '@angular/core';
import {DataService} from '../common/data.service';
import {HelperService} from '../common/helper.service';
import { ConfirmComponent } from '../common/confirm.component';
import { DialogService } from "ng2-bootstrap-modal";

@Component({
	selector: 'app-stacks',
	templateUrl: './stacks.component.html',
	styleUrls: ['./stacks.component.css']
})
export class StacksComponent implements OnInit {
	private largestStack: string;
	private threadCnt: string;
	private threads: any;
	private errorMessage : string;
	private threadMax : any;
	private optionThreadStacks: any;
	private dataThreadStacks: any;

	constructor(private dataService: DataService, private helper: HelperService,private dialogService:DialogService) { }

	ngOnInit() {
		this.getData();
		this.dataThreadStacks = [];
		this.optionThreadStacks = {
				chart: {
					type: 'multiBarHorizontalChart',
					x: (d) => {return d.name},
					y: (d) => {return d.value},
					height: 300,
					margin: {
						top: 20,
						right: 50,
						bottom: 40,
						left: 75
					},
					showValues: true,
					tooltips: true,
					transitionDuration: 350,
					showControls:false,
					stacked: true,
					yAxis: {
						tickFormat: (d) => {return this.helper.humanReadable(d,1,"B",false)},
						axisLabel: "Calls"
					}
				}
			};
	}

	genThreadMaxData() {
		var formattedData = [];
		var values = [];
		for (var j in this.threadMax)
		{
			values.push({
				name: j,
				value: this.threadMax[j]
			});
		}

		formattedData.push({
			key: "Stacks",
			values: values
		});
		console.log(formattedData);
		return formattedData;
	}

	extract() {
		this.threadCnt = this.threads.length;
		this.threadMax = [];
		var max = 0;

		//scan all threads
		for (var i in this.threads) {
			var localMax = 0;
			var sizeArray = this.threads[i].size;
			for (var j in  sizeArray)
				if (sizeArray[j] > localMax)
					localMax = sizeArray[j];
			this.threadMax[i] = localMax;
			if (localMax > max) {
				max = localMax;
			}
		}

		//compute largest stack
		this.largestStack = this.helper.humanReadable(max,1,"B",false);
	}

	getData() {
		this.dataService.getStackSize().subscribe(
			resp => {
				this.threads = resp;
				this.extract();
				this.dataThreadStacks = this.genThreadMaxData();
		  },
		  error => this.errorMessage = <any>error
		);
	}

	genCSV(entry:string) {
		if (entry == "threadStacks") {
			var out = "#ThreadID	Stack\n";
			for (var i in this.threadMax)
				out+=i+"\t"+this.threadMax[i]+"\n";
			return out;
		}
	}

	genJSON(entry:string) {
		if (entry == "threadStacks") {
			return JSON.stringify(this.threadMax,null,'\t');
		}
	}

	showDataCSV(title:string,entry:string) {
		let disposable = this.dialogService.addDialog(ConfirmComponent, {
			title:title,
			message:this.genCSV(entry)})
	}

	showDataJSON(title:string,entry:string) {
		let disposable = this.dialogService.addDialog(ConfirmComponent, {
			title:title,
			message:this.genJSON(entry)})
	}

	showDataSVG(id:string) {
		// Get the d3js SVG element
		var tmp = document.getElementById(id);
		var svg = tmp.getElementsByTagName("svg")[0];
		// Extract the data as SVG text string
		var svg_xml = (new XMLSerializer).serializeToString(svg);
		// save
		var blob = new Blob([svg_xml], {type: "image/svg+xml"});
		saveAs(blob, id+".svg");
	}
}
