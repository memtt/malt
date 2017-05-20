import { Component, OnInit } from '@angular/core';
declare var d3: any;
declare var saveAs: any;
import { NvD3Module } from 'angular2-nvd3';
import {DataService} from '../common/data.service';
import { ConfirmComponent } from '../common/confirm.component';
import { DialogService } from "ng2-bootstrap-modal";
import {HelperService} from '../common/helper.service';

@Component({
  selector: 'app-threads',
  templateUrl: './threads.component.html',
  styleUrls: ['./threads.component.css']
})
export class ThreadsComponent implements OnInit {
	errorMessage: string;
	private d3: any;
	public optionsTime:any;
	public optionsSum:any;
	public optionsCount:any;
	public dataCount:any;
	public dataSum:any;
	public dataTime:any;
	public data:any;
	public allocFuncs:any;
	public ticksPerSecond:number;

	constructor(private dataService: DataService,private dialogService:DialogService,private helper: HelperService) {
		this.d3 = d3;
		this.allocFuncs = ["malloc","free","calloc","realloc","posix_memalign","aligned_alloc","memalign","valloc","pvalloc"];
	}
	
	genData(data:any,metric:string) {
		var formattedData = [];
		var allocFuncs = this.allocFuncs;
		for (var i in allocFuncs)
		{
			var values = [];
			for (var j in data)
			{
				values.push({
					name:"Thread "+j,
					value: data[j][allocFuncs[i]][metric]
				});
			}
			
			formattedData.push({
				key: allocFuncs[i],
				values: values
			});
		}
		console.log(formattedData);
		return formattedData;
	}
	
	genCSV(entry:string) {
		var out = "threadId";
		for (var i in this.allocFuncs)
			out+="\t"+this.allocFuncs[i];
		for (var i in this.data) {
			out+="\n"+i;
			for (var j in this.allocFuncs)
				out += "\t"+this.data[i][this.allocFuncs[j]][entry];
		}
		return out;
	}
	
	genJSON(entry:string) {
		var out = [];
		for (var i in this.data) {
			out[i] = {};
			for (var j in this.allocFuncs)
				out[i][this.allocFuncs[j]] = this.data[i][this.allocFuncs[j]][entry];
		}
		return JSON.stringify(out,null,'\t');
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
	
	getData() {
		this.dataService.getThreadsStats().subscribe(
			resp => {
				this.dataCount = this.genData(resp.data,"count");
				this.dataSum = this.genData(resp.data,"sumSize");
				this.dataTime = this.genData(resp.data,"sumTime");
				this.optionsTime.chart.height = 30 * resp.data.length;
				this.optionsCount.chart.height = 30 * resp.data.length;
				this.optionsSum.chart.height = 30 * resp.data.length;
				this.data = resp.data;
				this.ticksPerSecond = resp.ticksPerSecond;
			},
			error => this.errorMessage = <any>error
		);
	}

	ngOnInit() {
		this.optionsCount = {
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
						tickFormat: (d) => {return this.helper.humanReadable(d,1,"",false)},
						axisLabel: "Calls"
					}
				}
			}
		
		this.optionsTime = {
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
						tickFormat: (d) => {return this.helper.humanReadable(d / this.ticksPerSecond,1,"s",false)},
						axisLabel: "Time (cycles)"
					}
				}
			}
		
		this.optionsSum = {
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
						axisLabel: "Sum"
					}
				}
			}

		this.dataSum = [];
		this.dataTime = [];
		this.dataCount = [];
		this.getData();
	}
}
