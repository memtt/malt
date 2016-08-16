function MaltPageTimeline()
{
	///////////////////////////////////// MAIN ////////////////////////////////////
	//declare module to manage malt home page
	var maltModule = angular.module('malt.page.sources',[]);
	var svgId = 0;
	var cur = this;
	
	//main controler of the page
	var pageCtrl = maltCtrl.controller('malt.page.sources.ctrl',['$scope','$routeParams','$http',function($scope,$routeParams,$http) {
		$scope.functions = [];
		$scope.file = "No source file...";
		$scope.function = "";
		$scope.selectedDetails = null;
		$scope.selector = new MaltSelector();
		$scope.editor = new MaltSourceEditor('malt-source-editor',$scope.selector);
		$scope.callStacks = new MaltCallStacksView('malt-alloc-stacks-tree',$scope.selector);

		// Is the source file present? Was it found?
		$scope.hasSources = function() 
		{
			return ($scope.file == "No source file..." || $scope.file == "??" 
				|| $scope.file == '' || $scope.file == undefined);
		}
		
		// Event handler for maltFunctionSelector onSelect event
		$scope.onFunctionSelectEvent = function(data) {
			$scope.file = data.file;
			$scope.function = data.function;
			$scope.editor.moveToFileFunction(data.file,data.function);
			$scope.callStacks.updateFunc(data.function);
			$scope.selectedDetails = data;
		};
		
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
						$scope.onFunctionSelectEvent(data[i]);
			}
			if ($routeParams.metric != undefined)
				$scope.selector.selectMetric({key:$routeParams.metric});
		});
	}]);
}

var maltPageTimeline = new MaltPageTimeline();
