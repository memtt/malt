import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { HttpModule } from '@angular/http';

import { AppComponent } from './app.component';
import { HomeComponent } from './home/home.component';
import { RouterModule }   from '@angular/router';

import {DataService} from './common/data.service';
import {HelperService} from './common/helper.service';
import { D3Service } from 'd3-ng2-service'; 

@NgModule({
  declarations: [
    AppComponent,
    HomeComponent
  ],
  imports: [
    BrowserModule,
    FormsModule,
    HttpModule,
    RouterModule.forRoot([
        { path: 'home', component: HomeComponent },
            { path: '', component: HomeComponent },
            { path: '**', component: HomeComponent }
    ])
  ],
  providers: [D3Service,DataService,HelperService],
  bootstrap: [AppComponent]
})
export class AppModule { }
