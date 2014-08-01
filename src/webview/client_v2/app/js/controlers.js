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

mattCtrl.controller('matt.ctrl.funcs', ['$scope', '$http',
	function ($scope, $http) {
		$scope.funcs = [];
		$http.get('data/funcs.json').success(function(data) {
			$scope.funcs = data;
			$scope.selected = 'virt';
			$scope.sortReverse = false;
		});
		
		//click on lines
		$scope.selectFunc = function(func) {
			alert('click on func : '+func.function);
			$scope.code = func.function;
		};
		
		//select view
		$scope.toogleSelected = function() {
			if ($scope.selected == 'virt')
				$scope.selected = 'phys';
			else
				$scope.selected = 'virt';
		};
		
		$scope.toogleSort = function() {
			$scope.sortReverse = !$scope.sortReverse;
		};
		
		//codemirror
		$scope.editorOptions = {
			lineWrapping : true,
			lineNumbers: true,
			readOnly: 'nocursor',
			mode: 'xml',
		};
		
		// pagination
		$scope.curPage = 0;
		$scope.pageSize = 5;

		$scope.numberOfPages = function() {
			return Math.ceil($scope.funcs.length / $scope.pageSize);
		};

	}]);

mattCtrl.filter('pagination', function() {
	return function(input, start)
	{
	start = +start;
	return input.slice(start);
	};
});

mattCtrl.directive('mattfunclist',function() {
	return {
		restrict: 'EA',
		templateUrl: 'partials/funcs-directive.html',
		replace: true,
		scope: {
			funcs: '=',
			selected: '@',
			limit: '@'
		},
		link: function ($scope) {
			$scope.selectFunc = function(func) {
				alert('click on func : '+func.function);
			};
		}
	};
});