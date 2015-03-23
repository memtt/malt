'use strict';

describe('malt.ctrl',function() {
	describe('malt.ctrl.page1',function() {
		beforeEach(module('malt.controlers'));
		beforeEach(function(){
			localStorage.clear();
		});
		
		it('should have "coucou" in scope', inject(function ($controller) {
			var scope = {};
			var ctrl = $controller('malt.ctrl.page1',{$scope:scope});
			
			expect(scope.test).toBe('coucou');
		}));
	});
});