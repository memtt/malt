/**
 * This file contains all the directives needed across different pages.
 * This encourages reuse of components rather than repeating code across controllers.
 */
(function() {
	var directives = angular.module('malt.directives',[]);

	directives.filter('pagination', function() {
		return function(input, start)
		{
			start = +start;
			return input.slice(start);
		};
	});
	
	// Directive to print function lists
	directives.directive('maltsourcefunclist',function() {
		return {
			restrict: 'EA',
			templateUrl: 'partials/source-func-list.html',
			replace: true,
			scope: {
				functions: '=',
				metric: '=',
				limit: '=',
				inclusive: '=',
				order:'=',
				ratio:'=',
				query:'=',
				selector:'=',
				compact: '@',
				onSelect:'&'
			},
			link: function ($scope) {
				$scope.funcMetrics = new MaltFuncMetrics();
				
				//select values for sort
				$scope.getValue = function (x) {
					return $scope.selector.getValue(x);
				}

				$scope.computeRef = function() {
					return $scope.funcMetrics.getRef($scope.functions,$scope.metric);
				}
				
				$scope.isSelectedFunc = function(x) {
					return (x.function == $scope.$parent.function);
				}
				
				$scope.getValueRatio = function(x) {
					return $scope.selector.getValueRatio(x);
				}
	
				//format value for print
				$scope.getFormattedValue = function(x) {
					if ($scope.ratio)
					{
						return $scope.getValueRatio(x).toFixed(1)+"%";
					} else {
						return $scope.funcMetrics.getFormattedValue(x,$scope.metric,$scope.inclusive);
					}
				}
				
				//check reverse
				$scope.isReversedOrder = function () {
					return ($scope.selector.isReversedOrder());
				}
				
				$scope.useHiddenClass = function (x)  {
					return ($scope.getValue(x) == 0);
				}
		
				$scope.viewFilter = function(x)
				{
					return $scope.selector.filter(x);
				}
				
				$scope.selectFunc = function(x) {
					$scope.onSelect({data: x});
				}
			}
		};
	});

	// Directive to show the function selector
	directives.directive('maltFunctionSelector', function() {
		return {
			restrict: 'EA',
			templateUrl: 'partials/function-selector.html',
			replace: true,
			scope: {
				selector: '=',
				functions: '=',
				metric: '=',
				onSelect: '&'
			},
			link: function ($scope) {
				var funcMetric = new MaltFuncMetrics();
				$scope.metricList = funcMetric.getMetricList();
				$scope.inclusive = true;
				$scope.limit = 15;
				$scope.order = funcMetric.maltMetrics[$scope.metric || 'alloc.count'].defaultOrder;
				$scope.ratio = false;
				$scope.query='';
				$scope.maxSize = 4;

				$scope.orderBtnStatus = function() {
					return ($scope.selector.order == 'asc');
				}

				$scope.functionSelectEvent = function(data) {
					$scope.onSelect({data: data});
				};

				$scope.metricSelectEvent = function(data) {
					$scope.metric = data.key;
					$scope.selector.selectMetric(data);
				};
			}
		};
	})

	// Directive to show a memory table for a selected function or line
	directives.directive('maltMemoryTable', function() {
		return {
			restrict: 'EA',
			templateUrl: 'partials/memory-table.html',
			replace: true,
			scope: {
				selectedDetails: '=details'
			},
			link: function ($scope) {
				$scope.formatValue = function(value,unit) {
					if (value == undefined)
						return maltHelper.humanReadable(0,1,unit,false);
					else
						return maltHelper.humanReadable(value,1,unit,false);
				}

				$scope.formatRoundedRatio = function(value,divider,unit) {
					if (value == undefined || divider == undefined || divider == 0)
						return maltHelper.humanReadable(0,1,unit,false);
					else
						return $scope.formatValue(Math.round(value/divider),unit);
				}

				$scope.hasExlusiveValues = function() {
					if ($scope.selectedDetails == undefined || $scope.selectedDetails.own == undefined)
						return true;
					else
						return !($scope.selectedDetails.own.alloc.count > 0 && $scope.selectedDetails.own.alloc.count > 0);
				}
			}
		}
	});
})();