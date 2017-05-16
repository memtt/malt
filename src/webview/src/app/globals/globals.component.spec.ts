import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { GlobalsComponent } from './globals.component';

describe('GlobalsComponent', () => {
  let component: GlobalsComponent;
  let fixture: ComponentFixture<GlobalsComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ GlobalsComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(GlobalsComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should be created', () => {
    expect(component).toBeTruthy();
  });
});
