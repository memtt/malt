'use strict';

describe('matt.ctrl',function() {
	describe('matt.ctrl.page1',function() {
		beforeEach(module('matt.controlers'));
		beforeEach(function(){
			localStorage.clear();
		});
		
		it('should have "coucou" in scope', inject(function ($controller) {
			var scope = {};
			var ctrl = $controller('matt.ctrl.page1',{$scope:scope});
			
			expect(scope.test).toBe('coucou');
		}));
	});
});