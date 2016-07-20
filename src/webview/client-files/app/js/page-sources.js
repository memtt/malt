function MaltPageTimeline()
{
	///////////////////////////////////// MAIN ////////////////////////////////////
	//declare module to manage malt home page
	var maltModule = angular.module('malt.page.sources',[]);
	var svgId = 0;
	var cur = this;
	
	//main controler of the page
	var pageCtrl = maltCtrl.controller('malt.page.sources.ctrl',['$scope','$routeParams','$http',function($scope,$routeParams,$http) {
		//manage function list view
		$scope.functions = [];
		$scope.funcMetrics = new MaltFuncMetrics();
		$scope.metric = 'alloc.count';
		$scope.inclusive = true;
		$scope.limit = 15;
		
		$scope.currentPage = 1;
		$scope.totalItems = 20;
		$scope.maxSize = 8;
		$scope.maxSize = 4;
		
		$scope.ratio = false;
		$scope.query='';
		$scope.order = $scope.funcMetrics.maltMetrics[$scope.metric].defaultOrder;
		$scope.metricList = $scope.funcMetrics.getMetricList();
		$scope.file = "No source file...";
		$scope.selector = new MaltSelector();
		$scope.editor = new MaltSourceEditor('malt-source-editor',$scope.selector);
		$scope.callStacks = new MaltCallStacksView('malt-alloc-stacks-tree',$scope.selector);
		
		$scope.hasSources = function() 
		{
			return ($scope.file == "No source file..." || $scope.file == "??");
		}
		
		$scope.orderBtnStatus = function()
		{
			return ($scope.selector.order == 'asc');
		}
		
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
			return $scope.funcMetrics.maltMetrics[$scope.metric].name;
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
			$scope.function = e.function;
			$scope.editor.moveToFileFunction(e.file,e.function);
			$scope.callStacks.updateFunc(e.function);
			$scope.selectedDetails = e;
// 			detailView.render(details);
// 			//callStacks.updateFunc(details.function);
// 			callStacks.clear();
		});
		
		$scope.hasExlusiveValues = function()
		{
			if ($scope.selectedDetails == undefined || $scope.selectedDetails.own == undefined)
				return true;
			else
				return !($scope.selectedDetails.own.alloc.count > 0 && $scope.selectedDetails.own.alloc.count > 0);
		}
		
		$scope.formatValue = function(value,unit)
		{
			if (value == undefined)
				return maltHelper.humanReadable(0,1,unit,false);
			else
				return maltHelper.humanReadable(value,1,unit,false);
		}

		$scope.formatRoundedRatio = function(value,divider,unit)
		{
			//console.log(value);
			if (value == undefined || divider == undefined || divider == 0)
				return maltHelper.humanReadable(0,1,unit,false);
			else
				return $scope.formatValue(Math.round(value/divider),unit);
		}
		
		//dispatch update of view mode (metric...)
		$scope.selector.onChange = function()
		{
			//TODO use a better way to update the call stack to avoid to redraw it completly (and close it which is bad for user)
			$scope.callStacks.updateFunc($scope.function);
			$scope.editor.redrawAnnotations();
		}
		
		//dispatch click on annotations
		$scope.editor.onClick = function(details)
		{
			//$("#malt-source-filename").text(details.file);
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
// 			alert(JSON.stringify(info));
// 			$("#malt-source-filename").text(location.file);
			$scope.editor.moveToFileFunction(location.file,location.function);
			$scope.file = location.file;
			$scope.function = location.function;
			$scope.$apply();
		}
		
		//fetch function list datas
		maltDataSource.loadFlatFunctionStats($http,function(data) {
			$scope.functions = data;
			$scope.selector.setData(data);
			if ($routeParams.func != undefined)
			{
				for (var i in data)
					if (data[i].function == $routeParams.func)
						$scope.$broadcast('selectFunctionFromFuncList',data[i]);
			}
			if ($routeParams.metric != undefined)
				$scope.selector.selectMetric({key:$routeParams.metric});
		});
	}]);
	
	pageCtrl.filter('pagination', function() {
		return function(input, start)
		{
			start = +start;
			return input.slice(start);
		};
	});
	
	//directive to print function lists
	pageCtrl.directive('maltsourcefunclist',function() {
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
					return $scope.selector.filter(x);
				}
				
				$scope.selectFunc = function(x) {
					$scope.$parent.$broadcast('selectFunctionFromFuncList',x);
				}
			}
		};
	});
}

var maltPageTimeline = new MaltPageTimeline();
