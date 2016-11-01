import { Component, OnInit } from '@angular/core';
import {DataService} from '../common/data.service';
import * as d3 from 'd3';

@Component({
	selector: 'app-home',
	templateUrl: './home.component.html',
	styleUrls: ['./home.component.css']
})

export class HomeComponent implements OnInit {
	message: string;

	constructor(private _dataService: DataService) { }

	ngOnInit() {
		this.message = this._dataService.getMessage();
	}
	
	ngAfterViewInit(){
		console.log("afterViewInit() called");
		d3.selectAll("h1").style("background-color", "yellow");
	}
}
