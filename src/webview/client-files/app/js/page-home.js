function MaltPageHome()
{
	var cur = this;
	var defaultData = {
		run: {
			exe: '',
			command: '',
			tool: '',
			date: '',
			runtime: 0,
		},
		system: {
			ticksPerSecond: 1,
			totalMemory:1,
			freeMemory: {
				start:1,
				min:0,
				max:1
			},
			swapMemory: {
				start:0,
				min:0,
				max:0
			},
			cachedMemory: {
				start:0,
				min:0,
				max:0
			}
		},
		summary: {
			peakPhysicalMemory: 1,
			peakVirtualMemory: 1,
			peakRequestedMemory: 1,
			peakInternalMemory:1,
			peakSegmentCount: 1,
			totalAllocatedMemory: 1,
			allocCount: 1,
			maxAllocSize: 1,
			meanAllocSize: 1,
			minAllocSize: 1,
			peakAllocRate: 1,
			peakAllocCountRate: 1,
			recyclingRatio: 1,
			leakedMem:0,
			leakedCount:0,
			largestStack:1,
			globalVarMem:1,
			tlsMem:1
		},
		summaryWarnings: {
			/*peakPhysicalMemory:['Take care of large number for this !']*/
		}
	};

	//declare summary view entries
	var summaryViewEntries = [
		{
			key:'peakPhysicalMemory',
			name:'Physical memory peak',
			level: 1,
			help:"Peak of total physical memory obtained by tracking /proc/self/statm. It contain all allocated objects, global variables and binary code.",
			format: function(x) {return maltHelper.humanReadable(x,1,'B',false);}
		},{
			key:'peakVirtualMemory',
			name:'Virtual memory peak',
			level: 1,
			help:"Peak of total virtual memory obtained by tracking /proc/self/statm. It contain all allocated objects, global variables and binary code.",
			format: function(x) {return maltHelper.humanReadable(x,1,'B',false);}
		},{
			key:'peakRequestedMemory',
			name:'Requested memory peak',
			level: 1,
			help:"Peak of requested memory managed by malloc, free... It contain all dynamically allocated objects.",
			format: function(x) {return maltHelper.humanReadable(x,1,'B',false);}
		},{
			key:'peakSegmentCount',
			name:'Alive object peak',
			level: 2,
			help:"Peak number of alive object allocated by malloc, calloc, realloc....",
			format: function(x) {return maltHelper.humanReadable(x,1,'',false);}
		},{
			key:'peakInternalMemory',
			name:'MALT peak memory',
			level: 2,
			help:"Peak of memory internally used by MALT.",
			format: function(x) {return maltHelper.humanReadable(x,1,'B',false);}
		},{
			type:'separator',
			level: 2
		},{
			key:'totalAllocatedMemory',
			name:'Cumulated memory allocations',
			level: 1,
			help:"Sum of all dyanmic memory allocation throuth malloc, calloc, realloc....",
			format: function(x) {return maltHelper.humanReadable(x,1,'B',false);}
		},{
			key:'allocCount',
			name:'Allocation count',
			level: 1,
			help:"Total number of all dyanmic memory allocation with malloc, calloc, realloc....",
			format: function(x) {return maltHelper.humanReadable(x,1,'',false);}
		},{
			key:'recyclingRatio',
			name:'Recycling ratio',
			level: 1,
			help:"Ratio of cumulated memory allocations over maximum requested memory. Ideally it might be 1 if the program allocate the memory on start and free on end.",
			format: function(x) {return x.toFixed(1);}
		},{
			type:'separator',
			level: 2
		},{
			key:'minAllocSize',
			name:'Smallest allocations',
			level: 2,
			help:"The smallest request size received by malloc, calloc...",
			format: function(x) {return maltHelper.humanReadable(x,1,'B',false);}
		},{
			key:'meanAllocSize',
			name:'Mean allocations',
			level: 2,
			help:"The mean request size received by malloc, calloc...",
			format: function(x) {return maltHelper.humanReadable(x,1,'B',false);}
		},{
			key:'maxAllocSize',
			name:'Largest allocations',
			level: 2,
			help:"The largest request size received by malloc, calloc...",
			format: function(x) {return maltHelper.humanReadable(x,1,'B',false);}
		},{
			type:'separator',
			level: 2
		},{
			key:'leakedMem',
			name:'Leaked memory',
			level: 1,
			help:"Total memory which has leaked (malloc, calloc... without free).",
			format: function(x) {return maltHelper.humanReadable(x,1,'B',false);}
		},{
			key:'leakedCount',
			name:'Leaked object count',
			level: 2,
			help:"Total number of allocated object which have leaked (malloc, calloc... without free).",
			format: function(x) {return maltHelper.humanReadable(x,1,'',false);}
		},{
			type:'separator',
			level: 2
		},{
			key:'largestStack',
			name:'Largest stack',
			level: 1,
			help:"Memory used by the largest stack seen during execution of threads.",
			format: function(x) {return maltHelper.humanReadable(x,1,'B',false);}
		},{
			key:'globalVarMem',
			name:'Global variables',
			level: 1,
			help:"Memory used by global variables.",
			format: function(x) {return maltHelper.humanReadable(x,1,'B',false);}
		},{
			key:'tlsVarMem',
			name:'TLS variables',
			level: 1,
			help:"Memory used by TLS variables (already multiplied by number of thread of execution).",
			format: function(x) {return maltHelper.humanReadable(x,1,'B',false);}
		},{
			key:'numGblVar',
			name:'Global variable count',
			level: 2,
			help:"Total number of global and TLS variables from your application and libraries.",
			format: function(x) {return maltHelper.humanReadable(x,1,"",false);}
		},{
			type:'separator',
			level: 2
		},{
			key:'peakAllocRate',
			name:'Peak allocation rate',
			level: 1,
			help:"Peak of memory allocation rate (in memory size).",
			format: function(x) {return maltHelper.humanReadable(x,1,'B/s',false);}
		},{
			key:'peakAllocCountRate',
			name:'Peak allocation call rate',
			level: 2,
			help:"Peak of memory allocation call rate.",
			format: function(x) {return maltHelper.humanReadable(x,1,'/s',false);}
		}
	];

	//declare module to manage malt home page
	var maltModule = angular.module('malt.page.home',[]);
	
	//main controler of the page
	var homeCtrl = maltCtrl.controller('malt.page.home.ctrl',['$scope','$http',function($scope,$http) {
		//setup data
		$scope.summaryViewEntries = summaryViewEntries;
		$scope.displaySummaryLevel = 1;
		$scope.displaySummaryHelp = false;
		$scope.summaryData = defaultData;
		
		//fetch summaryData
		maltDataSource.loadGlobalSummary($http,function(data) {
			$scope.summaryData = data;
		});
		
		//fetch function list datas
		maltDataSource.loadFlatFunctionStats($http,function(data) {
			$scope.functions = data;
		});
		
		//provide short function to check if has warning to enable color class on tab lines
		$scope.hasSummaryWarnings = function(key) {
			if ($scope.summaryData.summaryWarnings[key] == undefined)
				return false;
			else
				return ($scope.summaryData.summaryWarnings[key].length > 0);
		}
		
		//convert times
		$scope.getFormattedExecTime = function() {
			return maltHelper.ticksToHourMinSec($scope.summaryData.run.runtime,$scope.summaryData.system.ticksPerSecond);
		}
		
		//convert CPU freq
		$scope.getFormattedCpuFreq = function() {
			return maltHelper.humanReadable($scope.summaryData.system.ticksPerSecond,1,'Hz',false);
		}

		//return allocator, we can later extrat name from lib
		$scope.getAllocator = function() {
			return $scope.summaryData.run.allocator;
		}
		
		//convert CPU freq
		$scope.getFormattedTotalMemory = function() {
			return maltHelper.humanReadable($scope.summaryData.system.totalMemory,1,'b',false);
		}

		//format values
		$scope.getFormattedValue = function(entry) {
			if (entry == undefined || entry.type == 'separator' || entry.key == undefined)
				return '';
			else
				return entry.format($scope.summaryData.summary[entry.key]);
		}
		
		$scope.getFormattedValueFromKey = function(key) {
			for (var i in summaryViewEntries)
			{
				if (summaryViewEntries[i].key == key)
					return $scope.getFormattedValue(summaryViewEntries[i]);
			}
			return "";
		}
		
		//manage toogle button
		$scope.toogleSummaryDetails = function() {
			if ($scope.displaySummaryLevel == 1)
				$scope.displaySummaryLevel = 2;
			else
				$scope.displaySummaryLevel = 1;
		}
	}]);
	
	//filter to limit list of summary entries by default
	homeCtrl.filter('summaryLevel', function() {
		return function(input, level, hasSummaryWarnings)
		{
			var res=[];
			for (var i in input)
			{
				if (input[i].level == undefined || input[i].level <= level || hasSummaryWarnings[input[i].key] != undefined)
					res.push(input[i]);
			}
			return res;
		};
	});
	
	//directive to print function lists
	homeCtrl.directive('malthomefunclist',function() {
		return {
			restrict: 'EA',
			templateUrl: 'partials/home-func-list.html',
			replace: true,
			scope: {
				functions: '=',
				metric: '@',
				limit: '@',
				inclusive: '@'
			},
			link: function ($scope) {
				$scope.funcMetrics = new MaltFuncMetrics();
				
				//select values for sort
				$scope.getValue = function (x) {
					return $scope.funcMetrics.getValue(x,$scope.metric,($scope.inclusive == 'true'));
				}

				$scope.computeRef = function() {
					return $scope.funcMetrics.getRef($scope.functions,$scope.metric);
				}
				
				$scope.getValueRatio = function(x) {
					return (100 *$scope.getValue(x)) / $scope.computeRef();
				}
	
				//format value for print
				$scope.getFormattedValue = function(x) {
					return $scope.funcMetrics.getFormattedValue(x,$scope.metric,($scope.inclusive == 'true'));
				}
				
				//check reverse
				$scope.getReverse = function () {
					return ($scope.funcMetrics.maltMetrics[$scope.metric].defaultOrder == 'desc');
				}
				
				$scope.useHiddenClass = function (x)  {
					return ($scope.getValue(x) == 0);
				}
			}
		};
	});
}

//init and export
var maltPageHome = new MaltPageHome();
