import { Component, OnInit } from '@angular/core';
import { NvD3Module } from 'angular2-nvd3';
import { DataService } from '../common/data.service';
import { ConfirmComponent } from '../common/confirm.component';
import { DialogService } from "ng2-bootstrap-modal";
import { HelperService } from '../common/helper.service';

@Component({
	selector: 'app-sizes',
	templateUrl: './sizes.component.html',
	styleUrls: ['./sizes.component.css']
})
export class SizesComponent implements OnInit {
	private summary: any;
	private dataMostUsed: any;
	private optionMostUsed : any;
	private errorMessage: string;
	private data : any;

	constructor(private dataService: DataService,private dialogService:DialogService,private helper: HelperService) {
		this.summary = {
			meanSize: 0,
			mostUsedSize: 0,
			smallestSize: 0,
			largestSize: 0
		};
		this.dataMostUsed = [];
	}

	buildSummary() {
		//calc mean
		var sum = 0;
		var count = 0;
		for (var i in this.data) {
			sum += this.data[i] * (+i);
			count++;
		}
		this.summary.meanSize = this.helper.humanReadable(sum/count,1,"B",false);

		//search most unsigne
		var mostSize = 0;
		var mostCount = 0;
		for (var i in this.data) {
			if (this.data[i] > mostCount) {
				mostCount = this.data[i];
				mostSize = +i;
			}
		}
		this.summary.mostUsedSize = this.helper.humanReadable(mostSize,1,"B",false);

		//min/max
		var min = -1;
		var max = 0;
		for (var i in this.data) {
			if (min == -1 || +i < min)
				min = +i;
			if (+i > max)
				max = +i;
		}
		this.summary.smallestSize = this.helper.humanReadable(min,1,"B",false);
		this.summary.largestSize = this.helper.humanReadable(max,1,"B",false);
	}

	getData() {
		this.dataService.getAllocSizeDistr().subscribe(
			resp => {
				this.data = resp;
				this.buildSummary();
				this.dataMostUsed = resp;
				console.log(this.dataMostUsed);
			},
			error => this.errorMessage = <any>error
		);
	}

	ngOnInit() {
		this.dataMostUsed = {1:1,2:2,3:3};
		this.getData();
	}

}
