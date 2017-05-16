import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { CalltreeComponent } from './calltree.component';

describe('CalltreeComponent', () => {
  let component: CalltreeComponent;
  let fixture: ComponentFixture<CalltreeComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ CalltreeComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(CalltreeComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should be created', () => {
    expect(component).toBeTruthy();
  });
});
