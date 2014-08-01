var mattApp = angular.module('matt.app', [
  'ngRoute',
  'matt.controlers',
  'matt.anim',
  'ui.codemirror',
  'matt.page.home',
]);

mattApp.config(['$routeProvider',
	function($routeProvider) {
		$routeProvider.
		when('/home', {
			templateUrl: 'partials/home.html',
			controller: 'matt.page.home.ctrl',
			pageKey: 'home'
		}).
		when('/help', {
			templateUrl: 'partials/help.html',
			pageKey: 'help'
		}).
		when('/page2', {
			templateUrl: 'partials/page-2.html',
			controller: 'matt.ctrl.page2',
			pageKey: 'page2'
		}).
		when('/funcs', {
			templateUrl: 'partials/funcs.html',
			controller: 'matt.ctrl.funcs',
			pageKey: 'funcs'
		})
		.otherwise({
			redirectTo: '/home'
		});
	}]);

// .run(function ($rootScope, $http, $route) {
// 		$rootScope.$on("$routeChangeSuccess", function (angularEvent, 	currentRoute, previousRoute) {
// 			alert(currentRoute.pageKey);
// 			$rootScope.activePageKey = currentRoute.pageKey;
// 		});
// 	});;

