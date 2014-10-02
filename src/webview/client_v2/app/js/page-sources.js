function MattPageTimeline()
{
	///////////////////////////////////// MAIN ////////////////////////////////////
	//declare module to manage matt home page
	var mattModule = angular.module('matt.page.sources',[]);
	var svgId = 0;
	var cur = this;
	
	//main controler of the page
	var pageCtrl = mattCtrl.controller('matt.page.sources.ctrl',['$scope','$routeParams','$http',function($scope,$routeParams,$http) {
		//manage function list view
		$scope.functions = [];
		$scope.funcMetrics = new MattFuncMetrics();
		$scope.metric = 'alloc.count';
		$scope.inclusive = true;
		$scope.limit = 30;
		$scope.ratio = false;
		$scope.query='';
		$scope.order = mattMetrics[$scope.metric].defaultOrder;
		$scope.metricList = $scope.funcMetrics.getMetricList();
		$scope.file = "No source file...";
		$scope.selector = new MattSelector();
		$scope.editor = new MattSourceEditor('matt-source-editor',$scope.selector);
		$scope.callStacks = new MattCallStacksView('matt-alloc-stacks-tree',$scope.selector);

		//fetch function list datas
		$http.get('/flat.json').success(function(data) {
			$scope.functions = data;
			$scope.selector.setData(data);
		});
		
		//buttons
		$scope.toogleOrder = function()
		{
			$scope.order = ($scope.order == 'asc')?'desc':'asc';
		}
		
		$scope.toogleRatio = function()
		{
			$scope.ratio =  !$scope.ratio;
		}
		
		$scope.toogleInclusive = function()
		{
			$scope.inclusive = !$scope.inclusive;
		}
		
		$scope.getCurMetricName = function()
		{
			return mattMetrics[$scope.metric].name;
		}
		
		$scope.selectMetric = function(metric)
		{
			$scope.metric = metric.key;
		}
		
		//click on function
		$scope.$on('selectFunctionFromFuncList',function(event,e) {
			console.log(e);
// 			$scope.editor.
			$scope.file = e.file;
			$scope.editor.moveToFileFunction(e.file,e.function);
			$scope.callStacks.updateFunc(e.function);
			$scope.selectedDetails = e;
// 			detailView.render(details);
// 			//callStacks.updateFunc(details.function);
// 			callStacks.clear();
		});
		
		$scope.formatValue = function(value,unit)
		{
			if (value == undefined)
				return mattHelper.humanReadable(0,1,unit,false);
			else
				return mattHelper.humanReadable(value,1,unit,false);
		}

		$scope.formatRoundedRatio = function(value,divider,unit)
		{
			//console.log(value);
			if (value == undefined || divider == undefined || divider == 0)
				return mattHelper.humanReadable(0,1,unit,false);
			else
				return $scope.formatValue(Math.round(value/divider),unit);
		}
		
		//dispatch update of view mode (metric...)
		$scope.selector.onChange = function()
		{
// 			leftFuncList.render();
			$scope.editor.redrawAnnotations();
		}
		
		//dispatch click on annotations
		$scope.editor.onClick = function(details)
		{
			//$("#matt-source-filename").text(details.file);
			//detailView.render(details);
			$scope.callStacks.update(details.file,details.line);
			$scope.selectedDetails = details;
			$scope.$apply();
		}
		
		//distpatch click on stack tree
		$scope.callStacks.onClick = function(location,info)
		{
			console.log("click on call stack");
			console.log(location);
			console.log(info);
			//alert(JSON.stringify(info));
// 			$("#matt-source-filename").text(location.file);
			$scope.editor.moveToFileFunction(location.file,location.function);
			$scope.file = location.file;
		}
	}]);
	
	pageCtrl.filter('pagination', function() {
		return function(input, start)
		{
			start = +start;
			return input.slice(start);
		};
	});
	
	//directive to print function lists
	pageCtrl.directive('mattsourcefunclist',function() {
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
				selector:'='
			},
			link: function ($scope) {
				$scope.funcMetrics = new MattFuncMetrics();
				
				//select values for sort
				$scope.getValue = function (x) {
					return $scope.selector.getValue(x);
				}

				$scope.computeRef = function() {
					return $scope.funcMetrics.getRef($scope.functions,$scope.metric);
				}
				
				$scope.getValueRatio = function(x) {
					return $scope.selector.getValueRatio(x);
// 					return (100 *$scope.getValue(x)) / $scope.computeRef();
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
					return ($scope.getValue(x) > 0 && ($scope.query == '' || x.function.indexOf($scope.query) > -1));
				}
				
				$scope.selectFunc = function(x) {
					$scope.$parent.$broadcast('selectFunctionFromFuncList',x);
				}
			}
		};
	});
}

var mattPageTimeline = new MattPageTimeline();
