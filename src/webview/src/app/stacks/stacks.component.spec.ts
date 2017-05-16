import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { StacksComponent } from './stacks.component';

describe('StacksComponent', () => {
  let component: StacksComponent;
  let fixture: ComponentFixture<StacksComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ StacksComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(StacksComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should be created', () => {
    expect(component).toBeTruthy();
  });
});
