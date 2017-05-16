import { Component, OnInit } from '@angular/core';
declare var d3: any;
import { NvD3Module } from 'angular2-nvd3';

@Component({
  selector: 'app-threads',
  templateUrl: './threads.component.html',
  styleUrls: ['./threads.component.css']
})
export class ThreadsComponent implements OnInit {

	private d3: any;
	public options:any;
	public data:any;

	constructor() {
		this.d3 = d3;
		
	
	}

  ngOnInit() {
  
	this.options = {
			chart: {
				type: 'lineChart',
				useInteractiveGuideline: true,
				height: 450,
				transitionDuration: 350,
				showLegend: false,
				margin: {
				top: 20,
				right: 20,
				bottom: 40,
				left: 55
				},
				x: (d) => { return d.x; },
				y: (d) => { return d.y; },
				xScale: this.d3.time.scale(),
				xAxis: {
				ticks: this.d3.time.months,
				tickFormat: (d) => {
					return this.d3.time.format('%b')(new Date(d));
				}
				},
				yAxis: {
				axisLabel: 'Gross volume',
				tickFormat: (d) => {
					if (d == null) {
						return 0;
					}
					return this.d3.format('.02f')(d);
				},
				axisLabelDistance: 400
				}
			}
			}

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
				]
			}
			];
  }

}
