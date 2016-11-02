import {Injectable} from '@angular/core';
import { Http, Response } from '@angular/http';
import { Observable }     from 'rxjs/Observable';
import 'rxjs/add/operator/map';

export class MattRunInfo {
	executionTime: number;
};

@Injectable()
export class DataService {
	private baseUrl = "/api";

	constructor(private http: Http) {
	}

	getRunInfo() : Observable<MattRunInfo> {
		return this.http.get(this.baseUrl+"/run-infos.json")
			.map(resp => resp.json());
	}
}
