//decalre controlers for MATT
var mattCtrl = angular.module('matt.controlers',[]);

mattCtrl.controller('matt.ctrl.help',['$scope',function($scope) {
	//codemirror
	$scope.editorOptions = {
		lineWrapping : false,
		lineNumbers: false,
		readOnly: 'nocursor',
		mode: 'clike',
	};
}]);

mattCtrl.controller('matt.ctrl.home',['$scope',function($scope) {
}]);

mattCtrl.controller('matt.ctrl.page1',['$scope',function($scope) {
	$scope.test = 'coucou';
}]);

mattCtrl.controller('matt.ctrl.page2',['$scope',function($scope) {
	$scope.test = 'coucou';
}]);

mattCtrl.controller('matt.ctrl.nav',['$scope','$route',function($scope,$route) {
	$scope.route = $route;
}]);
