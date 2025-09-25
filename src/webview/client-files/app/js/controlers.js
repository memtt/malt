/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 03/2015
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/client-files/app/js/controlers.js
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
***********************************************************/
//decalre controlers for MALT
var maltCtrl = angular.module('malt.controlers',[]);

maltCtrl.controller('malt.ctrl.help',['$scope',function($scope) {
	//codemirror
	$scope.editorOptions = {
		lineWrapping : false,
		lineNumbers: false,
		readOnly: 'nocursor',
		mode: 'clike',
	};
}]);

maltCtrl.controller('malt.ctrl.home',['$scope',function($scope) {
}]);

maltCtrl.controller('malt.ctrl.page1',['$scope',function($scope) {
	$scope.test = 'coucou';
}]);

maltCtrl.controller('malt.ctrl.page2',['$scope',function($scope) {
	$scope.test = 'coucou';
}]);

maltCtrl.controller('malt.ctrl.nav',['$scope','$route',function($scope,$route) {
	$scope.route = $route;
}]);
