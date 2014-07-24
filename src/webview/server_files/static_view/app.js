/* app */
var mattApp = angular.module('mattApp', [
  'ngRoute',
  'mattControllers'
]);

mattApp.config(['$routeProvider',
	function($routeProvider) {
		$routeProvider.
		when('/summary', {
			templateUrl: 'summary.html',
			controller: 'mattSummaryCtrl'
		}).
		otherwise({
			redirectTo: '/summary'
		});
}]);
