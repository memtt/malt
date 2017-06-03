declare var d3: any;
declare var saveAs: any;

import { Component, OnInit } from '@angular/core';
import {DataService} from '../common/data.service';
import {HelperService} from '../common/helper.service';
import { ConfirmComponent } from '../common/confirm.component';
import { DialogService } from "ng2-bootstrap-modal";
import { Injectable, NgZone } from '@angular/core';

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
	private optionTime: any;
	private dataTime: any;
	private ticksPerSecond: number;
	private selectedThread: string;
	private data: any;
	private dataStack : any;
	private optionStack: any;

	constructor(
		private dataService: DataService,
		private helper: HelperService,
		private dialogService:DialogService,
		private zone: NgZone) {
	}

	ngOnInit() {
		this.getData();
		this.dataThreadStacks = [];
		this.dataTime = [];
		this.dataStack = [];
		this.selectedThread = "";
		this.setupCharts();
	}

	setupCharts() {
		var cur = this;
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
					},
					//thanks to https://bridge360blog.com/2016/03/07/adding-and-handling-click-events-for-nvd3-graph-elements-in-angular-applications/
					callback: (chart) => {
						chart.multibar.dispatch.on('elementClick', function(e){
							cur.zone.run(() => {
								//console.log('elementClick in callback', e.data);
								cur.selectedThread = e.data.name;
								console.log(e.data.name);
								cur.dataTime = cur.getTimeData();
								console.log("===============");
								cur.getThreadDetils(+cur.selectedThread);
							});
						});
					}
				}
			};

			this.optionTime = {
					chart: {
						type: 'lineChart',
						height: 300,
						margin: {
							top: 20,
							right: 50,
							bottom: 40,
							left: 75
						},
						tooltips: true,
						transitionDuration: 350,
						showControls:false,
						useInteractiveGuideline:true,
						yAxis: {
							tickFormat: (d) => {return this.helper.humanReadable(d,1,"B",false)},
							axisLabel: "Memory"
						},
						xAxis: {
							tickFormat: (d) => {return this.helper.humanReadableTimes(d/this.ticksPerSecond)},
							axisLabel: "Time"
						}
					}
				};

			this.optionStack = {
					chart: {
						type: 'multiBarHorizontalChart',
						x: (d) => {return d.name},
						y: (d) => {return d.value},
						height: 300,
						margin: {
							top: 20,
							right: 50,
							bottom: 40,
							left: 150
						},
						showValues: true,
						tooltips: true,
						transitionDuration: 350,
						showControls:false,
						stacked: true,
						yAxis: {
							tickFormat: (d) => {return this.helper.humanReadable(d,1,"B",false)},
							axisLabel: "Stack memory"
						},
						//thanks to https://bridge360blog.com/2016/03/07/adding-and-handling-click-events-for-nvd3-graph-elements-in-angular-applications/
						callback: (chart) => {
							chart.multibar.dispatch.on('elementClick', function(e){
								cur.zone.run(() => {
									console.log('elementClick in callback', e.data);
								});
							});
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
		for (var i in this.data.max) {
			this.threadMax[i] = this.data.max[i].size;
			if (this.threadMax[i] > max)
				max = this.threadMax[i];
		}

		//compute largest stack
		this.largestStack = this.helper.humanReadable(max,1,"B",false);
	}

	getTimeData() {
		var ticks = this.data.ticksPerSecond;

		var out = [];
		var cnt = 0;
		for (var i in this.data.data) {
			var points = [];
			var d = this.data.data[i];
			var steps = d.steps;
			var points = [];

			for (var j in d.size)
				points.push({x: (+j)*(+steps), y: d.size[j]});

			if (i == this.selectedThread || this.selectedThread == "") {
				console.log("add "+i+" => "+this.selectedThread);
				out.push({
					key: i,
					values: points,
					area : true
				});
			}

			//we display max 8 at same time
			cnt++;
			if (cnt > 8)
				break;
		}
		return out;
	}

	getThreadDetils(threadId:number) {
		console.log("Details of threadId : "+threadId);
		var cur = this;
		this.dataService.getStackSizeDetails(threadId).subscribe(
			resp => {
				cur.zone.run(() => {
					var formatted = [];
					for (var i in resp)
						formatted.push({name:resp[i].location.function,value:resp[i].mem,location:resp[i].location});
					cur.dataStack = [
							{
								"key": "Stack functions",
								"values": formatted
							}
					  ];
					 this.optionStack.chart.height = 70 * resp.length;
				});
		  },
		  error => { cur.zone.run(() => this.errorMessage = <any>error); }
		);
	}

	getData() {
		this.dataService.getStackSize().subscribe(
			resp => {
				this.data = resp;
				this.threads = resp.data;
				this.ticksPerSecond = resp.ticksPerSecond;
				this.extract();
				this.dataTime = this.getTimeData();
				this.dataThreadStacks = this.genThreadMaxData();
				this.getThreadDetils(0);
		  },
		  error => this.errorMessage = <any>error
		);
	}

	genCSVThreadStack() {
		var out = "#ThreadID	Stack\n";
		for (var i in this.threadMax)
			out+=i+"\t"+this.threadMax[i]+"\n";
		return out;
	}

	genJsonTime() {
		//build data with common merged ids
		var tmp = {};
		for (var i in this.dataTime)
			for (var j in this.dataTime[i].values) {
				var d = this.dataTime[i];
				if (tmp[d.values[j].x] == undefined)
					tmp[d.values[j].x] = {};
				tmp[d.values[j].x][i] = d.values[j].y;
			}

		//fill missing and gen data lines
		var last = {};
		for (var i in tmp) {
			var local = [];
			for (var j in this.dataTime) {
				if (tmp[i][j] == undefined) {
					if (last[j] == undefined)
						tmp[i][j] = 0;
					else
						tmp[i][j] = last[j];
				} else {
					last[j] = tmp[i][j];
				}
				local.push(tmp[i][j]);
			}
			tmp[i] = local;
		}

		return tmp;
	}

	genCSVTime() {
		//setup titles
		var out = "#Time";
		for (var i in this.dataTime)
			out += " "+this.dataTime[i].key;
		out += "\n";

		//get data
		var tmp = this.genJsonTime();

		//gen lines
		for (var i in tmp) {
			out += (+i/this.ticksPerSecond)+"\t";
			for (var j in tmp[i])
				out+="\t"+tmp[i][j];
			out += "\n";
		}

		return out;
	}

	genCSV(entry:string) {
		switch (entry) {
			case "threadStacks":
				return this.genCSVThreadStack();
			case "time":
				return this.genCSVTime();
		}
	}

	genJSON(entry:string) {
		switch(entry) {
			case "threadStacks":
				return JSON.stringify(this.threadMax,null,'\t');
			case "time":
				return JSON.stringify(this.genJsonTime(),null,'\t');
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
