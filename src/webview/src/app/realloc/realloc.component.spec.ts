import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { ReallocComponent } from './realloc.component';

describe('ReallocComponent', () => {
  let component: ReallocComponent;
  let fixture: ComponentFixture<ReallocComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ ReallocComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(ReallocComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should be created', () => {
    expect(component).toBeTruthy();
  });
});
