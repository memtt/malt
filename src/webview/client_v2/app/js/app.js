var mattApp = angular.module('matt.app', [
  'ngRoute',
  'matt.controlers',
  'matt.anim',
  'ui.codemirror',
  'matt.page.home',
  'matt.page.globalVars',
  'matt.page.allocSizeDistr',
  'matt.page.realloc',
  'matt.page.stackPeaks'
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
		when('/stackPeaks', {
			templateUrl: 'partials/stack-peaks.html',
			controller: 'matt.page.stackPeaks.ctrl',
			pageKey: 'stackPeaks'
		}).
		when('/realloc', {
			templateUrl: 'partials/realloc.html',
			controller: 'matt.page.realloc.ctrl',
			pageKey: 'realloc'
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

