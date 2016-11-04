import { Component, OnInit} from '@angular/core';
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

	private displaySummaryLevel:number = 1;
	private displaySummaryHelp: boolean = false;
	private summaryData = {};

	//declare summary view entries
	private summaryViewEntries: any = [];

	constructor(private dataService: DataService, private helper: HelperService) {
		this.fillSummaryViewEntries();
	}

	ngOnInit() {
		this.getData();
	}

	getData() {
		this.dataService.getRunInfo().subscribe(
			resp => this.runInfo = resp,
			error => this.errorMessage = <any>error
		);
		this.dataService.getSummaryInfo().subscribe(
			resp => this.summaryData = resp,
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

	getFormattedValue(entry:any) {
		console.log(this.summaryData);
		console.log(this.summaryData[entry.key]);
		if (entry == undefined || entry.type == 'separator' || entry.key == undefined || this.summaryData[entry.key] == undefined)
			return '';
		else
			return entry.format(this.summaryData[entry.key]);
	}

	getFormattedValueFromKey(key:string) {
		for (var i in this.summaryViewEntries)
		{
			if (this.summaryViewEntries[i].key == key)
				return this.getFormattedValue(this.summaryViewEntries[i]);
		}
		return "";
	}

	getFormattedTotalMemory() {
		return 0;
	}

	getFormattedCpuFreq() {
		return this.helper.humanReadable(this.runInfo.globals.ticksPerSecond,1,'Hz',false);
	}

	toogleSummaryDetails() {
		if (this.displaySummaryLevel == 1)
			this.displaySummaryLevel = 2;
		else
			this.displaySummaryLevel = 1;
	}

	getSummaryViewEntries() {
		var self = this;
		return this.summaryViewEntries.filter(it => it.level <= self.displaySummaryLevel);
	}


	fillSummaryViewEntries() {
		var self = this;
		this.summaryViewEntries = [
			{
				key:'peakPhysicalMemory',
				name:'Physical memory peak',
				level: 1,
				help:"Peak of total physical memory obtained by tracking /proc/self/statm. It contain all allocated objects, global variables and binary code.",
				format: function(x) {return self.helper.humanReadable(x,1,'B',false);}
			},{
				key:'peakVirtualMemory',
				name:'Virtual memory peak',
				level: 1,
				help:"Peak of total virtual memory obtained by tracking /proc/self/statm. It contain all allocated objects, global variables and binary code.",
				format: function(x) {return self.helper.humanReadable(x,1,'B',false);}
			},{
				key:'peakRequestedMemory',
				name:'Requested memory peak',
				level: 1,
				help:"Peak of requested memory managed by malloc, free... It contain all dynamically allocated objects.",
				format: function(x) {return self.helper.humanReadable(x,1,'B',false);}
			},{
				key:'peakSegmentCount',
				name:'Alive object peak',
				level: 2,
				help:"Peak number of alive object allocated by malloc, calloc, realloc....",
				format: function(x) {return self.helper.humanReadable(x,1,'',false);}
			},{
				key:'peakInternalMemory',
				name:'MALT peak memory',
				level: 2,
				help:"Peak of memory internally used by MALT.",
				format: function(x) {return self.helper.humanReadable(x,1,'B',false);}
			},{
				type:'separator',
				level: 2
			},{
				key:'totalAllocatedMemory',
				name:'Cumulated memory allocations',
				level: 1,
				help:"Sum of all dyanmic memory allocation throuth malloc, calloc, realloc....",
				format: function(x) {return self.helper.humanReadable(x,1,'B',false);}
			},{
				key:'allocCount',
				name:'Allocation count',
				level: 1,
				help:"Total number of all dyanmic memory allocation with malloc, calloc, realloc....",
				format: function(x) {return self.helper.humanReadable(x,1,'',false);}
			},{
				key:'recyclingRatio',
				name:'Recycling ratio',
				level: 1,
				help:"Ratio of cumulated memory allocations over maximum requested memory. Ideally it might be 1 if the program allocate the memory on start and free on end.",
				format: function(x) {return x.toFixed(1);}
			},{
				type:'separator',
				level: 2
			},{
				key:'minAllocSize',
				name:'Smallest allocations',
				level: 2,
				help:"The smallest request size received by malloc, calloc...",
				format: function(x) {return self.helper.humanReadable(x,1,'B',false);}
			},{
				key:'meanAllocSize',
				name:'Mean allocations',
				level: 2,
				help:"The mean request size received by malloc, calloc...",
				format: function(x) {return self.helper.humanReadable(x,1,'B',false);}
			},{
				key:'maxAllocSize',
				name:'Largest allocations',
				level: 2,
				help:"The largest request size received by malloc, calloc...",
				format: function(x) {return self.helper.humanReadable(x,1,'B',false);}
			},{
				type:'separator',
				level: 2
			},{
				key:'leakedMem',
				name:'Leaked memory',
				level: 1,
				help:"Total memory which has leaked (malloc, calloc... without free).",
				format: function(x) {return self.helper.humanReadable(x,1,'B',false);}
			},{
				key:'leakedCount',
				name:'Leaked object count',
				level: 2,
				help:"Total number of allocated object which have leaked (malloc, calloc... without free).",
				format: function(x) {return self.helper.humanReadable(x,1,'',false);}
			},{
				type:'separator',
				level: 2
			},{
				key:'largestStack',
				name:'Largest stack',
				level: 1,
				help:"Memory used by the largest stack seen during execution of threads.",
				format: function(x) {return self.helper.humanReadable(x,1,'B',false);}
			},{
				type:'separator',
				level: 2
			},{
				key:'globalVarMem',
				name:'Global variables',
				level: 1,
				help:"Memory used by global variables.",
				format: function(x) {return self.helper.humanReadable(x,1,'B',false);}
			},{
				key:'tlsVarMem',
				name:'TLS variables',
				level: 1,
				help:"Memory used by TLS variables (already multiplied by number of thread of execution).",
				format: function(x) {return self.helper.humanReadable(x,1,'B',false);}
			},{
				key:'numGblVar',
				name:'Global variable count',
				level: 2,
				help:"Total number of global and TLS variables from your application and libraries.",
				format: function(x) {return self.helper.humanReadable(x,1,"",false);}
			},{
				type:'separator',
				level: 2
			},{
				key:'peakAllocRate',
				name:'Peak allocation rate',
				level: 1,
				help:"Peak of memory allocation rate (in memory size).",
				format: function(x) {return self.helper.humanReadable(x,1,'B/s',false);}
			},{
				key:'peakAllocCountRate',
				name:'Peak allocation call rate',
				level: 2,
				help:"Peak of memory allocation call rate.",
				format: function(x) {return self.helper.humanReadable(x,1,'/s',false);}
			}
		];
	}
}
