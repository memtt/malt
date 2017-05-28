import {Injectable} from '@angular/core';
import { Http, Response } from '@angular/http';
import { Observable }     from 'rxjs/Observable';
import 'rxjs/add/operator/map';

@Injectable()
export class DataService {
	private baseUrl = "/api";

	constructor(private http: Http) {
	}

	getRunInfo() : Observable<any> {
		return this.http.get(this.baseUrl+"/run-infos.json")
			.map(resp => resp.json());
	}

	getSummaryInfo() : Observable<any> {
		return this.http.get(this.baseUrl+"/summary-infos.json")
			.map(resp => resp.json());
	}

	getThreadsStats() : Observable<any> {
		return this.http.get(this.baseUrl+"/thread-stats.json")
			.map(resp => resp.json());
	}

	getTimeline() : Observable<any> {
		return this.http.get(this.baseUrl+"/timeline.json")
			.map(resp => resp.json());
	}
}
