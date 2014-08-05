var mattApp = angular.module('matt.app', [
  'ngRoute',
  'matt.controlers',
  'matt.anim',
  'ui.codemirror',
  'matt.page.home',
  'matt.page.globalVars',
  'matt.page.allocSizeDistr'
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
		when('/globalVars', {
			templateUrl: 'partials/global-vars.html',
			controller: 'matt.page.globalVars.ctrl',
			pageKey: 'globalVars'
		}).
		when('/allocSizeDistr', {
			templateUrl: 'partials/alloc-size-distr.html',
			controller: 'matt.page.allocSizeDistr.ctrl',
			pageKey: 'allocSizeDistr'
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

