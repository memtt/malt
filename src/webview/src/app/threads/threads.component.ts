import { Component, OnInit } from '@angular/core';
declare var d3: any;
import { NvD3Module } from 'angular2-nvd3';
import {DataService} from '../common/data.service';

@Component({
  selector: 'app-threads',
  templateUrl: './threads.component.html',
  styleUrls: ['./threads.component.css']
})
export class ThreadsComponent implements OnInit {
	errorMessage: string;
	private d3: any;
	public options:any;
	public dataCount:any;
	public dataSum:any;
	public dataTime:any;

	constructor(private dataService: DataService) {
		this.d3 = d3;
	}
	
	genData(data:any,metric:string) {
		var formattedData = [];
		var allocFuncs = ["malloc","free","calloc","realloc","posix_memalign","aligned_alloc","memalign","valloc","pvalloc"];
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
	
	getData() {
		this.dataService.getThreadsStats().subscribe(
			resp => {
				this.dataCount = this.genData(resp,"count");
				this.dataSum = this.genData(resp,"sumSize");
				this.dataTime = this.genData(resp,"sumTime");
				this.options.chart.height = 30 * resp.length;
			},
			error => this.errorMessage = <any>error
		);
	}

	ngOnInit() {
		this.options = {
				chart: {
					type: 'multiBarHorizontalChart',
					x: (d) => {return d.name},
					y: (d) => {return d.value},
					height: 300,
					margin: {
						top: 30, 
						right: 0,
						bottom: 50, 
						left: 0
					},
					showValues: true,
					tooltips: true,
					transitionDuration: 350,
					showControls:false,
					stacked: true
				}
			}

		this.dataSum = [];
		this.dataTime = [];
		this.dataCount = [];
		this.getData();
	}
}
