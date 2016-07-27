/**
 * Provide a call-tree graph which shows all the function calls,
 * the allocations alongwith UI for navigation.
 */
function MaltPageCallTree() 
{
	function createSvgGraphForTree(svg, focusedNode) {
		var parser = new DOMParser();
		var svg = parser.parseFromString(svg, "image/svg+xml");
		svg.documentElement.id = "svggraph";
		document.getElementById("svggraph") && document.getElementById("svggraph").remove();
   		document.getElementById("mynetwork").appendChild(svg.documentElement);

   		return svgPanZoom('#svggraph', {
            zoomEnabled: true,
            controlIconsEnabled: false,
            fit: true,
            center: true,
            dblClickZoomEnabled: false,
            maxZoom: 1000
        });
	}

	maltCtrl.controller('malt.page.calltree.ctrl',['$scope','$routeParams','$http', function($scope,$routeParams,$http) {
		var tree = null;
		var panZoomControls = null;
		var prvFunc = null;

		$scope.nodeData = null;
		$scope.navigationHistory = [];
		$scope.navigationFwdHistory = [];
		$scope.functions = [];
		$scope.selector = new MaltSelector();
		$scope.selectedDetails = null;

		$scope.filterHeight = "-1";
		$scope.filterDepth = "3";
		$scope.filterNodeCost = "1";

		function navigateTo(nodeId) {
			$scope.navigationHistory.push({
				nodeId: nodeId,
			});
			$scope.navigationFwdHistory = [];
		}

		function navigateBack() {
			$scope.navigationFwdHistory.push($scope.navigationHistory.pop());
			return $scope.navigationHistory[$scope.navigationHistory.length - 1];
		}

		function navigateForward() {
			var state = $scope.navigationFwdHistory.pop();
			$scope.navigationHistory.push(state);
			return state;
		}

		function reloadGraph() {
			if(!$scope.nodeData)
				return;

			maltDataSource.getCallTreeData($scope.nodeData.nodeId, $scope.filterDepth, 
				$scope.filterHeight, $scope.filterNodeCost, null, function(data) {
					$scope.$apply(function() {
						$scope.nodeData = data;
					});
					redrawGraph();
				});			
		}

		function redrawGraph() {
			if(!$scope.nodeData)
				return;

			panZoomControls = createSvgGraphForTree($scope.nodeData.svg, $scope.nodeData.nodeId);

			$('.node').on('dblclick', function(e) {
				var nodeId = parseInt($(this).find('title').html().substr(4));
				maltDataSource.getCallTreeData(nodeId, $scope.filterDepth, 
					$scope.filterHeight, $scope.filterNodeCost, null, function(data) {
						$scope.$apply(function() {
							$scope.nodeData = data;
							navigateTo(nodeId);
						});
						redrawGraph();
					});
			});
		}

		$scope.onNavigateBackEvent = function() {
			if(!$scope.nodeData)
				return;
			var state = navigateBack();
			maltDataSource.getCallTreeData(state.nodeId, $scope.filterDepth, 
				$scope.filterHeight, $scope.filterNodeCost, null, function(data) {
					$scope.$apply(function() {
						$scope.nodeData = data;
					});
					redrawGraph();
				});
		}

		$scope.onNavigateForwardEvent = function() {
			if(!$scope.nodeData)
				return;
			var state = navigateForward();
			maltDataSource.getCallTreeData(state.nodeId, $scope.filterDepth, 
				$scope.filterHeight, $scope.filterNodeCost, null, function(data) {
					$scope.$apply(function() {
						$scope.nodeData = data;
					});
					redrawGraph();
				});
		}

		$scope.onFunctionSelectEvent = function(data) {
			if(prvFunc == data.function)
				return;
			else
				prvFunc = data.function;

			$scope.selectedDetails = data;
			maltDataSource.getCallTreeData(null, $scope.filterDepth, 
				$scope.filterHeight, $scope.filterNodeCost, data.function, function(nodata) {
					if(nodata.error) {
						alert("Could not find the selected function.");
					} else {
						$scope.$apply(function() {
							$scope.nodeData = nodata;
						});
						navigateTo($scope.nodeData.nodeId);
						redrawGraph();
					}
				});
		};

		$scope.$watch('filterHeight', reloadGraph);
		$scope.$watch('filterDepth', reloadGraph);
		$scope.$watch('filterNodeCost', reloadGraph);

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

		maltDataSource.getCallTreeData(null, $scope.filterDepth, 
			$scope.filterHeight, $scope.filterNodeCost, '_start', function(data) {
				$scope.nodeData = data;
				redrawGraph();
				navigateTo($scope.nodeData.nodeId);
			});
	}]);
}

var maltPageCallTree = new MaltPageCallTree();