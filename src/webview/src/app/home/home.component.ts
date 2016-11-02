import { Component, OnInit } from '@angular/core';
import {DataService} from '../common/data.service';
import * as d3 from 'd3';
import {HelperService} from '../common/helper.service';

@Component({
	selector: 'app-home',
	templateUrl: './home.component.html',
	styleUrls: ['./home.component.css']
})

export class HomeComponent implements OnInit {
	errorMessage: string;
	runInfo: any = {
		"run": {
			"formatVersion": 1,
			"tool": "",
			"date": "",
			"runtime": 0,
			"exe": "",
			"command": "",
			"hostname": ""
		},
		"globals": {
			"ticksPerSecond": 1,
			"peak": {
				"finalValue": 0,
				"peakTime": 0,
				"peak": 0
			},
			"totalSysMem": 0,
			"freeMemoryAtStart": 0,
			"cachedMemoryAtStart": 0,
			"maxThreads": 8
		}
	};

	constructor(private dataService: DataService, private helper: HelperService) { }

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
		return this.helper.ticksToHourMinSec(this.runInfo.run.runtime,this.runInfo.globals.ticksPerSecond);
	}

	getFormatedAllocationPeak() {
		return this.helper.humanReadable(this.runInfo.globals.peak.peak,1,"B",false);
	}

	getFormattedValueFromKey() {
		return 0;
	}

	getFormattedTotalMemory() {
		return 0;
	}

	getFormattedCpuFreq() {
		return this.helper.humanReadable(this.runInfo.globals.ticksPerSecond,1,'Hz',false);
	}
}
