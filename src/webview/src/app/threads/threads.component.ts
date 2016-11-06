import { Component, OnInit,ElementRef, ViewChild } from '@angular/core';
import {DataService} from '../common/data.service';
import {nvD3} from 'ng2-nvd3'
import * as d3 from 'd3';

@Component({
	selector: 'app-threads',
	templateUrl: './threads.component.html',
	styleUrls: ['./threads.component.css'],
	providers: [ ElementRef ]
})
export class ThreadsComponent implements OnInit {
	private threadsStats : any;
	private errorMessage : string;
	private options : any;
	private data : any;
	@ViewChild('container') element: ElementRef;

	constructor(private dataService:DataService) { }

	ngOnInit() {
		this.dataService.getThreadsStats().subscribe(
  			resp => this.threadsStats = resp,
  			error => this.errorMessage = <any>error
  	  	);
		this.setupChartOptions();
		this.setupChartData();
	}

	afterViewInit(){
		console.log("afterViewInit() called");
		//d3.select(this.elementRef.nativeElement).select("h1").style("background-color", "yellow");
	}

	setupChartOptions()
	{
		this.options = {
			chart: {
				type: 'discreteBarChart',
				height: 450,
				margin : {
					top: 20,
					right: 20,
					bottom: 50,
					left: 55
				},
				x: function(d){return d.label;},
				y: function(d){return d.value;},
				showValues: true,
				valueFormat: function(d){
					return d3.format(',.4f')(d);
				},
				duration: 500,
				xAxis: {
					axisLabel: 'X Axis'
				},
				yAxis: {
					axisLabel: 'Y Axis',
					axisLabelDistance: -10
				}
			}
		}
	}

	setupChartData()
	{
		this.data = [
			{
				key: "Cumulative Return",
				values: [
					{
						"label" : "A" ,
						"value" : -29.765957771107
					} ,
					{
						"label" : "B" ,
						"value" : 0
					} ,
					{
						"label" : "C" ,
						"value" : 32.807804682612
					} ,
					{
						"label" : "D" ,
						"value" : 196.45946739256
					} ,
					{
						"label" : "E" ,
						"value" : 0.19434030906893
					} ,
					{
						"label" : "F" ,
						"value" : -98.079782601442
					} ,
					{
						"label" : "G" ,
						"value" : -13.925743130903
					} ,
					{
						"label" : "H" ,
						"value" : -5.1387322875705
					}
				]
			}
		];
	}
}
