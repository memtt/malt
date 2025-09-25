/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 03/2015
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/test/unit/controllersSpec.js
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
***********************************************************/
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