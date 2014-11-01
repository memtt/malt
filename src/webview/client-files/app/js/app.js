var mattApp = angular.module('matt.app', [
  'ngRoute',
  'matt.controlers',
  'matt.anim',
  'ui.codemirror',
  'matt.page.home',
  'matt.page.globalVars',
  'matt.page.allocSizeDistr',
  'matt.page.realloc',
  'matt.page.stackPeaks',
  'matt.page.timeline',
  'matt.page.sources',
  'ui.bootstrap'
]);

mattApp.config(['$routeProvider',
	function($routeProvider) {
		$routeProvider.
		when('/home', {
			templateUrl: 'partials/home.html',
			controller: 'matt.page.home.ctrl',
			pageKey: 'home'
		}).
		when('/per-thread', {
			templateUrl: 'partials/per-thread.html',
			controller: 'matt.page.perthread.ctrl',
			pageKey: 'per-thread'
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
		}).
		when('/timeline', {
			templateUrl: 'partials/timeline.html',
			controller: 'matt.page.timeline.ctrl',
			pageKey: 'timeline'
		}).
		when('/sources', {
			templateUrl: 'partials/sources.html',
			controller: 'matt.page.sources.ctrl',
			pageKey: 'sources'
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

