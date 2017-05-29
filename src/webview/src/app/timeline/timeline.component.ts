declare var d3: any;
declare var saveAs: any;

import { Component, OnInit } from '@angular/core';
import { NvD3Module } from 'angular2-nvd3';
import { DataService } from '../common/data.service';
import { ConfirmComponent } from '../common/confirm.component';
import { DialogService } from "ng2-bootstrap-modal";
import { HelperService } from '../common/helper.service';

@Component({
  selector: 'app-timeline',
  templateUrl: './timeline.component.html',
  styleUrls: ['./timeline.component.css']
})
export class TimelineComponent implements OnInit {
	errorMessage: string;
	private d3: any;
	private data: any;
	private ticksPerSecond: any;
	private optionMemOverTime: any;
	private dataMemOverTime: any;
	private optionAliveChunks:any;
	private dataAliveChunks: any;
	private dataRateSize: any;
	private optionRateSize: any;
	private optionRateOps: any;
	private dataRateOps: any;
	private dataSysMem: any;
	private optionSysMem: any;
	private dataMaltMem: any;
	private optionMaltMem: any;

	constructor(private dataService: DataService,private dialogService:DialogService,private helper: HelperService) {
		this.d3 = d3;
		this.dataMemOverTime = [];
		this.dataAliveChunks = [];
		this.dataRateSize = [];
		this.dataRateOps = [];
		this.dataSysMem = [];
		this.optionSysMem = [];
		this.optionMaltMem = [];
		this.dataMaltMem = [];
	}

	genMemOverTime(data) {
		var ticks = data.ticksPerSecond;
		var steps = data.data.memStats.steps;
		var d = data.data.memStats;

		var req = [];
		for (var i in d.requestedByMalloc)
			req.push({x: (+i)*(+steps), y: d.requestedByMalloc[i]});

		var phys = [];
		for (var i in d.processPhysical)
			phys.push({x: (+i)*(+steps), y: d.processPhysical[i]});

		var virt = [];
		for (var i in d.processVirtual)
			virt.push({x: (+i)*(+steps), y: d.processVirtual[i]});

		return [{
			key: "Requested memory",
			values: req,
			area : true
		},{
			key: "Physical memory",
			values: phys,
			area: true
		},{
			key: "Virtual memory",
			values: virt,
			area: true
		}];
	}

	genChunksOverTime(data) {
		var ticks = data.ticksPerSecond;
		var steps = data.data.memStats.steps;
		var d = data.data.memStats;

		var req = [];
		for (var i in d.mallocSegments)
			req.push({x: (+i)*(+steps), y: d.mallocSegments[i]});

		return [{
			key: "Chunks",
			values: req,
			area : true
		}];
	}

	genMaltMem(data) {
		var ticks = data.ticksPerSecond;
		var steps = data.data.memStats.steps;
		var d = data.data.memStats;

		var req = [];
		for (var i in d.mattInternalMalloc)
			req.push({x: (+i)*(+steps), y: d.mattInternalMalloc[i]});

		return [{
			key: "Chunks",
			values: req,
			area : true
		}];
	}

	genRateSize(data) {
		var ticks = data.ticksPerSecond;
		var steps = data.data.memStats.steps;
		var d = data.data.opsBandwidth;

		var alloc = [];
		for (var i in d.allocSize)
			alloc.push({x: (+i)*(+steps), y: d.allocSize[i]});

		var free = [];
		for (var i in d.freeSize)
			free.push({x: (+i)*(+steps), y: d.freeSize[i]});

		return [{
			key: "Alloc",
			values: alloc,
			area : true
		},{
			key: "Free",
			values: free,
			area: true
		}];
	}

	genRateOps(data) {
		var ticks = data.ticksPerSecond;
		var steps = data.data.memStats.steps;
		var d = data.data.opsBandwidth;

		var alloc = [];
		for (var i in d.allocSize)
			alloc.push({x: (+i)*(+steps), y: d.allocCount[i]});

		var free = [];
		for (var i in d.freeSize)
			free.push({x: (+i)*(+steps), y: d.freeCount[i]});

		return [{
			key: "Alloc",
			values: alloc,
			area : true
		},{
			key: "Free",
			values: free,
			area: true
		}];
	}

	genSysMem(data) {
		var ticks = data.ticksPerSecond;
		var steps = data.data.memStats.steps;
		var d = data.data.systemStats;

		var free = [];
		for (var i in d.systemFreeMem)
			free.push({x: (+i)*(+steps), y: d.systemFreeMem[i]});

		var cache = [];
		for (var i in d.systemCachedMem)
			cache.push({x: (+i)*(+steps), y: d.systemCachedMem[i]});

		var swap = [];
			for (var i in d.systemSwapedMem)
				swap.push({x: (+i)*(+steps), y: d.systemSwapedMem[i]});

		return [{
			key: "Free memory",
			values: free,
			area : true
		},{
			key: "Disk cache",
			values: cache,
			area: true
		},{
			key: "Swap",
			values: swap,
			area: true
		}];
	}

	getData() {
		this.dataService.getTimeline().subscribe(
			resp => {
				this.data = resp.data;
				this.dataMemOverTime = this.genMemOverTime(resp);
				this.dataAliveChunks = this.genChunksOverTime(resp);
				this.dataRateSize = this.genRateSize(resp);
				this.dataRateOps = this.genRateOps(resp);
				this.dataSysMem = this.genSysMem(resp);
				this.dataMaltMem = this.genMaltMem(resp);
				this.ticksPerSecond = resp.ticksPerSecond;
			},
			error => this.errorMessage = <any>error
		);
	}

	ngOnInit() {
		this.optionMemOverTime = {
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
		this.optionAliveChunks = {
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
						yAxis: {
							tickFormat: (d) => {return this.helper.humanReadable(d,1,"",false)},
							axisLabel: "Chunks"
						},
						xAxis: {
							tickFormat: (d) => {return this.helper.humanReadableTimes(d/this.ticksPerSecond)},
							axisLabel: "Time"
						}
					}
				};

		this.optionRateSize = {
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
							showControls:true,
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

		this.optionRateOps = {
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
							showControls:true,
							yAxis: {
								tickFormat: (d) => {return this.helper.humanReadable(d,1,"Ops",false)},
								axisLabel: "Memory"
							},
							xAxis: {
								tickFormat: (d) => {return this.helper.humanReadableTimes(d/this.ticksPerSecond)},
								axisLabel: "Time"
							}
						}
					};

		this.optionSysMem = {
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

	this.optionMaltMem = {
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

		this.getData();
	}

	showDataCSV(title:string,entry:string) {
		let disposable = this.dialogService.addDialog(ConfirmComponent, {
			title:title,
			message:this.genCSV(entry)})
	}

	genCSV(entry:string) {
		var out = "#Time";
		var data = [];

		switch(entry) {
			case "memOverTime":
				data = this.dataMemOverTime;
				break;
			case "aliveChunks":
				data = this.dataAliveChunks;
				break;
			case "rateSize":
				data = this.dataRateSize;
				break;
			case "rateOps":
				data = this.dataRateOps;
				break;
			case "sysMem":
				data = this.dataSysMem;
				break;
			case "maltMem":
				data = this.dataMaltMem;
				break;
		}

		for (var i in data)
			out+="\t"+data[i].key;
		for (var i in data[0].values) {
			out+="\n"+data[0].values[i].x / this.ticksPerSecond;
			for (var j in data)
				out += "\t"+data[j].values[i].y;
		}
		return out;
	}

	genJSON(entry:string) {
		var out = {};
		var data = [];

		switch(entry) {
			case "memOverTime":
				data = this.dataMemOverTime;
				break;
			case "aliveChunks":
				data = this.dataAliveChunks;
				break;
			case "rateSize":
				data = this.dataRateSize;
				break;
			case "rateOps":
				data = this.dataRateOps;
				break;
			case "sysMem":
				data = this.dataSysMem;
				break;
			case "maltMem":
				data = this.dataMaltMem;
				break;
		}

		for (var i in data[0].values) {
			var x = data[0].values[i].x / this.ticksPerSecond;
			out[x] = {};
			for (var j in data)
				out[x][data[j].key] = data[j].values[i].y;
		}
		return JSON.stringify(out,null,"\t");
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
