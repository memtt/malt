import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { HttpModule } from '@angular/http';

import { AppComponent } from './app.component';
import { HomeComponent } from './home/home.component';
import { RouterModule }   from '@angular/router';

import {DataService} from './common/data.service';
import {HelperService} from './common/helper.service';
//import { D3Service } from 'd3-ng2-service';
import { NvD3Module } from 'angular2-nvd3';
import { BootstrapModalModule } from 'ng2-bootstrap-modal';

import { ThreadsComponent } from './threads/threads.component';
import { TimelineComponent } from './timeline/timeline.component';
import { SourcesComponent } from './sources/sources.component';
import { CalltreeComponent } from './calltree/calltree.component';
import { StacksComponent } from './stacks/stacks.component';
import { SizesComponent } from './sizes/sizes.component';
import { ReallocComponent } from './realloc/realloc.component';
import { GlobalsComponent } from './globals/globals.component';
import { HelpComponent } from './help/help.component'; 
import { ConfirmComponent } from './common/confirm.component';


@NgModule({
  declarations: [
    AppComponent,
    HomeComponent,
    ThreadsComponent,
    TimelineComponent,
    SourcesComponent,
    CalltreeComponent,
    StacksComponent,
    SizesComponent,
    ReallocComponent,
    GlobalsComponent,
    HelpComponent,
    ConfirmComponent
  ],
  imports: [
    BrowserModule,
    FormsModule,
    HttpModule,
    NvD3Module,
    BootstrapModalModule,
    RouterModule.forRoot([
        { path: 'home', component: HomeComponent },
        { path: 'threads', component: ThreadsComponent },
        { path: 'timeline', component: TimelineComponent },
        { path: 'stacks', component: StacksComponent },
        { path: 'realloc', component: ReallocComponent },
        { path: 'help', component: HelpComponent },
        { path: 'sizes', component: SizesComponent },
        { path: 'calltree', component: CalltreeComponent },
        { path: 'sources', component: SourcesComponent },
            { path: '', component: HomeComponent },
            { path: '**', component: HomeComponent }
    ])
  ],
  providers: [
    //D3Service,
    DataService,
    HelperService
  ],
  entryComponents: [
    ConfirmComponent
  ],
  bootstrap: [AppComponent]
})
export class AppModule { }
