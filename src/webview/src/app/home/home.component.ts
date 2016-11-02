import { Component, OnInit } from '@angular/core';
import {DataService,MattRunInfo} from '../common/data.service';
import * as d3 from 'd3';

@Component({
	selector: 'app-home',
	templateUrl: './home.component.html',
	styleUrls: ['./home.component.css']
})

export class HomeComponent implements OnInit {
	errorMessage: string;
	runInfo: MattRunInfo;

	constructor(private dataService: DataService) { }

	ngOnInit() {
		this.getData();
	}

	getData() {
		this.dataService.getRunInfo().subscribe(
			resp => this.runInfo = resp,
			error => this.errorMessage = <any>error
		);
	}

	ngAfterViewInit(){
		console.log("afterViewInit() called");
		d3.selectAll("h1").style("background-color", "yellow");
	}

	//TODO
	getFormattedExecTime() {
		if (this.runInfo)
			return this.runInfo.executionTime;
		else
			return 0;
	}

	getFormattedValueFromKey() {
		return 0;
	}

	getFormattedTotalMemory() {
		return 0;
	}

	getFormattedCpuFreq() {
		return 0;
	}
}
