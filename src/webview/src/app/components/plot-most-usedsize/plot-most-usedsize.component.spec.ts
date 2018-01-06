import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { PlotMostUsedsizeComponent } from './plot-most-usedsize.component';

describe('PlotMostUsedsizeComponent', () => {
  let component: PlotMostUsedsizeComponent;
  let fixture: ComponentFixture<PlotMostUsedsizeComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ PlotMostUsedsizeComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(PlotMostUsedsizeComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should be created', () => {
    expect(component).toBeTruthy();
  });
});
