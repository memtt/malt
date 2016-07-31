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

		showLoader();

		function showLoader() {
			$('.loader').show();
		}

		function hideLoader() {
			$('.loader').hide();
		}

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

		function loadGraph(nodeId, shouldNavigate, shouldUpdateDetails) {
			if(!nodeId && !$scope.nodeData)
				return;

			nodeId = nodeId || $scope.nodeData.nodeId;
			showLoader();

			maltDataSource.getCallTreeData(nodeId, $scope.filterDepth, 
				$scope.filterHeight, $scope.filterNodeCost, null, function(data) {
					hideLoader();

					$scope.$apply(function() {
						$scope.nodeData = data;
						if(shouldNavigate)
							navigateTo(nodeId);
					});

					drawGraph();

					if(shouldUpdateDetails) {
						for (var i = 0; i < $scope.functions.length; i++) {
							if($scope.functions[i].function == data.function) {
								$scope.$apply(function() {
									$scope.selectedDetails = $scope.functions[i];
								});
							}
						}
					}
				});
		}

		function drawGraph() {
			if(!$scope.nodeData)
				return;

			panZoomControls = createSvgGraphForTree($scope.nodeData.svg, $scope.nodeData.nodeId);

			$('.node').on('dblclick', function(e) {
				var nodeId = parseInt($(this).find('title').html().substr(4));
				loadGraph(nodeId, true, true);
			});

			$('.node a').on('click', function(e) {
				e.preventDefault();
			})
		}

		$scope.onNavigateBackEvent = function() {
			if(!$scope.nodeData)
				return;
			var state = navigateBack();
			loadGraph(state.nodeId, false, true);
		}

		$scope.onNavigateForwardEvent = function() {
			if(!$scope.nodeData)
				return;
			var state = navigateForward();
			loadGraph(state.nodeId, false, true);
		}

		$scope.onFunctionSelectEvent = function(data) {
			if(prvFunc == data.function)
				return;
			else
				prvFunc = data.function;

			$scope.selectedDetails = data;
			showLoader();
			maltDataSource.getCallTreeData(null, $scope.filterDepth, 
				$scope.filterHeight, $scope.filterNodeCost, data.function, function(nodata) {
					hideLoader();
					if(nodata.error) {
						alert("Could not find the selected function.");
					} else {
						$scope.$apply(function() {
							$scope.nodeData = nodata;
						});
						navigateTo($scope.nodeData.nodeId);
						drawGraph();
					}
				});
		};

		$scope.$watch('filterHeight', function() { loadGraph(); });
		$scope.$watch('filterDepth', function() { loadGraph(); });
		$scope.$watch('filterNodeCost', function() { loadGraph(); });

		var deferredArray = [new $.Deferred(), new $.Deferred()];
		maltDataSource.loadFlatFunctionStats($http,function(data) {
			$scope.functions = data;
			$scope.selector.setData(data);
			deferredArray[0].resolve();
			// if ($routeParams.func != undefined)
			// {
			// 	for (var i in data)
			// 		if (data[i].function == $routeParams.func)
			// 			$scope.onFunctionSelectEvent(data[i]);
			// }
			// if ($routeParams.metric != undefined)
			// 	$scope.selector.selectMetric({key:$routeParams.metric});
		});

		showLoader();
		maltDataSource.getCallTreeData(null, $scope.filterDepth, 
			$scope.filterHeight, $scope.filterNodeCost, '_start', function(data) {
				hideLoader();
				$scope.nodeData = data;
				drawGraph();
				navigateTo($scope.nodeData.nodeId);
				deferredArray[1].resolve();
			});

		$.when(deferredArray).then(function() {
			console.log("hello", $scope.functions);
			for (var i = 0; i < $scope.functions.length; i++) {
				console.log("se", $scope.functions[i].function, $scope.nodeData.function);
				if($scope.functions[i].function == $scope.nodeData.function) {
					$scope.$apply(function() {
						$scope.selectedDetails = $scope.functions[i];
					});
					console.log("found");
					return;
				}
			}
		});

	}]);
}

var maltPageCallTree = new MaltPageCallTree();