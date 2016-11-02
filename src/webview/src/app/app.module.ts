import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { HttpModule, JsonpModule } from '@angular/http';

import { AppComponent } from './app.component';
import { HomeComponent } from './home/home.component';
import { RouterModule }   from '@angular/router';
import { ThreadsComponent } from './threads/threads.component';

import {DataService} from './common/data.service';
import {HelperService} from './common/helper.service';

@NgModule({
  declarations: [
    AppComponent,
    HomeComponent,
    ThreadsComponent
  ],
  imports: [
    BrowserModule,
    FormsModule,
    HttpModule,
    JsonpModule,
    RouterModule.forRoot([
      { path: 'home', component: HomeComponent },
      { path: 'threads', component: ThreadsComponent },
      { path: '', component: HomeComponent },
      { path: '**', component: HomeComponent }
    ])
  ],
  providers: [DataService,HelperService],
  bootstrap: [AppComponent]
})
export class AppModule { }
