var maltApp = angular.module('malt.app', [
  'ngRoute',
  'malt.directives',
  'malt.controlers',
  'malt.anim',
  'malt.page.home',
  'malt.page.globalVars',
  'malt.page.allocSizeDistr',
  'malt.page.realloc',
  'malt.page.stackPeaks',
  'malt.page.timeline',
  'malt.page.sources',
  'ui.bootstrap'
]);

maltApp.config(['$routeProvider',
	function($routeProvider) {
		$routeProvider.
		when('/home', {
			templateUrl: 'partials/home.html',
			controller: 'malt.page.home.ctrl',
			pageKey: 'home'
		}).
		when('/per-thread', {
			templateUrl: 'partials/per-thread.html',
			controller: 'malt.page.perthread.ctrl',
			pageKey: 'per-thread'
		}).
		when('/help', {
			templateUrl: 'partials/help.html',
			pageKey: 'help'
		}).
		when('/globalVars', {
			templateUrl: 'partials/global-vars.html',
			controller: 'malt.page.globalVars.ctrl',
			pageKey: 'globalVars'
		}).
		when('/allocSizeDistr', {
			templateUrl: 'partials/alloc-size-distr.html',
			controller: 'malt.page.allocSizeDistr.ctrl',
			pageKey: 'allocSizeDistr'
		}).
		when('/stackPeaks', {
			templateUrl: 'partials/stack-peaks.html',
			controller: 'malt.page.stackPeaks.ctrl',
			pageKey: 'stackPeaks'
		}).
		when('/realloc', {
			templateUrl: 'partials/realloc.html',
			controller: 'malt.page.realloc.ctrl',
			pageKey: 'realloc'
		}).
		when('/funcs', {
			templateUrl: 'partials/funcs.html',
			controller: 'malt.ctrl.funcs',
			pageKey: 'funcs'
		}).
		when('/timeline', {
			templateUrl: 'partials/timeline.html',
			controller: 'malt.page.timeline.ctrl',
			pageKey: 'timeline'
		}).
		when('/sources', {
			templateUrl: 'partials/sources.html',
			controller: 'malt.page.sources.ctrl',
			pageKey: 'sources'
		}).
		when('/calltree', {
			templateUrl: 'partials/calltree.html',
			controller: 'malt.page.calltree.ctrl',
			pageKey: 'calltree'
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

