import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { SizesComponent } from './sizes.component';

describe('SizesComponent', () => {
  let component: SizesComponent;
  let fixture: ComponentFixture<SizesComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ SizesComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(SizesComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should be created', () => {
    expect(component).toBeTruthy();
  });
});
