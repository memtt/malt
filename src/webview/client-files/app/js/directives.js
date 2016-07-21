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
	
	// directive to print function lists
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
				onSelect: '&'
			},
			link: function ($scope) {
				var funcMetric = new MaltFuncMetrics();
				$scope.metric = 'alloc.count';
				$scope.metricList = funcMetric.getMetricList();
				$scope.inclusive = true;
				$scope.limit = 15;
				$scope.order = funcMetric.maltMetrics[$scope.metric].defaultOrder;
				$scope.ratio = false;
				$scope.query='';
				$scope.maxSize = 4;

				$scope.orderBtnStatus = function() {
					return ($scope.selector.order == 'asc');
				}

				$scope.functionSelectEvent = function(data) {
					$scope.onSelect({data: data});
				};

			}
		};
	})
})();