var MATT_POWER_PS  = ['&nbsp;','K','M','G','T','P'];
var MATT_POWER_NPS = [' ','K','M','G','T','P'];
function mattHumanReadableValue(value,unit,protectedSpace)
{
		var power = 0;
		while (value >= 1024)
		{
			power++;
			value /= 1024;
		}

		var res;
		if (protectedSpace)
			res = value.toFixed(1) + " " + MATT_POWER_PS[power] + unit;
		else
			res = value.toFixed(1) + " " + MATT_POWER_NPS[power] + unit;

		return res;
}

var mattControllers = angular.module('mattControllers', []);

mattControllers.controller('mattSummaryCtrl', ['$scope', '$http',
	function ($scope, $http) {
		$http.get('/summary.json').success(function(data) {
			$scope.alert = function(m) {alert(m);};
			$scope.globalStats = {
				virtualMem:  mattHumanReadableValue(data.globalStats.virtualMem,'B'),
				physicalMem: mattHumanReadableValue(data.globalStats.physicalMem,'B'),
				requestedMem: mattHumanReadableValue(data.globalStats.requestedMem,'B'),
				cumulAllocs: mattHumanReadableValue(data.globalStats.cumulAllocs,'B'),
				recyclingRatio: mattHumanReadableValue(data.globalStats.cumulAllocs/data.globalStats.requestedMem,''),
				allocCount: mattHumanReadableValue(data.globalStats.count,''),
				/*peakObjectCount: ''*/
				smallestAlloc: mattHumanReadableValue(data.globalStats.minChunkSize,'B'),
				largestAlloc: mattHumanReadableValue(data.globalStats.maxChunkSize,'B'),
				largestStack: mattHumanReadableValue(data.globalStats.largestStack,'B'),
				/*leaks:''*/
			};
		});
	}]
);
