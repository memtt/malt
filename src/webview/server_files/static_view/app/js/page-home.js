var homeData = {
	run: {
		exe: 'AVBP',
		command: 'AVBP --run',
		tool: 'matt-0.1.0',
		date: '2013-23-14',
		runtime: 1358879,
		ticksPerSecond: 130000
	},
	system: {
		totalMemory:160000,
		freeMemory: {
			start:160000,
			min:160000,
			max:13000
		},
		swapMemory: {
			start:160000,
			min:160000,
			max:13000
		},
		cachedMemory: {
			start:160000,
			min:160000,
			max:13000
		}
	},
	summary: {
		peakPhysicalMemory: 160000,
		peakVirtualMemory: 160000,
		peakRequestedMemory: 160000,
		peakInternalMemory:16000,
		totalAllocatedMemory: 1600000,
		allocCount: 16000,
		maxAllocSize: 64,
		meanAllocSize: 16,
		minAllocSize: 1,
		peakAllocRate: 160000,
		peakAllocCountRate: 100,
	},
	summaryWarnings: {
		peakPhysicalMemory:['Take care of large number for this !']
	}
};


function MattPageHome()
{
	//declare summary view entries
	var summaryViewEntries = [
		{
			key:'peakPhysicalMemory',
			name:'Physical memory peak',
			help:'balblabl',
			format: function(x) {return x;}
		}
	];
	
	//declare module to manage matt home page
	var mattModule = angular.module('matt.page.home',[]);
	
	//main controler of the page
	mattCtrl.controller('matt.page.home.ctrl',['$scope',function($scope) {
		//setup data
		$scope.data = homeData;
		$scope.summaryViewEntries = summaryViewEntries;
		
		//provide short function to check if has warning to enable color class on tab lines
		$scope.hasSummaryWarnings = function(key) {
			if ($scope.data.summaryWarnings[key] == undefined)
				return false;
			else
				return ($scope.data.summaryWarnings[key].length > 0);
		}
		
		//convert times
		$scope.ticksToHourMinSec = function(value) {
			return mattHelper.ticksToHourMinSec(value,$scope.data.run.ticksPerSecond);
		}
	}]);
}

var mattPageHome = new MattPageHome();
