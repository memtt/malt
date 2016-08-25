/**
 * Provide a call-tree graph which shows all the function calls,
 * the allocations alongwith UI for navigation.
 */
function MaltPageCallTree() 
{
	function createSvgGraphForTree(svg) {
		if(!svg)
			return;
		
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
		$scope.selectedMetric = 'alloc.count';
		$scope.contextMenuNodeId = -1;
		$scope.contextMenuFunction = null;

		$scope.filterHeight = "-1";
		$scope.filterDepth = "3";
		$scope.filterNodeCost = "1";

		// TODO Refactor into a directive
		// Perhaps we should now create a directive for the draggable graph
		// because this is getting messy. Plus, altering the DOM directly
		// from a class is a big no no. 
		
	    $(document).mouseup(function (e) {
	    	if(e.which == 3)
	    		return;
	        $("#contextMenu").hide();
	        $("#contextMenuGeneral").hide();
	    });

	    $('#mynetwork').on('dblclick', '.node', function(e) {
	    	var nodeId = parseInt($(this).find('title').html().substr(4));
	    	loadGraph(nodeId, true, true);
	    });

	    $('#mynetwork').on('contextmenu', '.node', function(e) {
	    	var dis = $(this);
	    	$scope.$apply(function() {
	    		$scope.contextMenuNodeId = parseInt(dis.find('title').html().substr(4));;
	    		$scope.contextMenuFunction = dis.find('a').attr('xlink:title');
	    	});

	        $("#contextMenuGeneral").hide();
	    	$("#contextMenu").css({
	    		display: "block",
	    		left: e.pageX,
	    		top: e.pageY - $(".navbar").outerHeight() - 10
	        });

	        return false;
	    });

	    $('#mynetwork').on('contextmenu', function(e) {
	        $("#contextMenu").hide();
    		$("#contextMenuGeneral").css({
    			display: "block",
    			left: e.pageX,
    			top: e.pageY - $(".navbar").outerHeight() - 10
    	    });
    	    return false;
	    });

	    $('#mynetwork').on('click', '.node a', function(e) {
	    	e.preventDefault();
	    });

		// End of TODO Refactor into a directive

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
				$scope.filterHeight, $scope.filterNodeCost, null, $scope.selectedMetric, 
				$scope.selector.ratio, function(data) {
					hideLoader();

					$scope.$apply(function() {
						$scope.nodeData = data;
						if(shouldNavigate)
							navigateTo(nodeId);
					});

					drawGraph();

					if(shouldUpdateDetails && nodeId != -1) {
						for (var i = 0; i < $scope.functions.length; i++) {
							if($scope.functions[i].function == data.function) {
								$scope.$apply(function() {
									$scope.selectedDetails = $scope.functions[i];
								});
							}
						}
					}

					if(nodeId == -1) {
						$scope.$apply(function() {
							$scope.selectedDetails = null;
						});
					}
				});
		}

		function drawGraph() {
			if(!$scope.nodeData)
				return;

			panZoomControls = createSvgGraphForTree($scope.nodeData.svg);
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
				$scope.filterHeight, $scope.filterNodeCost, data.function, $scope.selectedMetric, 
				$scope.selector.ratio, function(nodata) {
					hideLoader();
					if(nodata.error && nodata.error.nodeNotFoundError) {
						alert("Could not find the selected function.");
					} else {
						$scope.$apply(function() {
							$scope.nodeData = nodata;
							navigateTo($scope.nodeData.nodeId);
						});
						drawGraph();
					}
				});
		};

		$scope.downloadSVGLink = function() {
			if(!$scope.nodeData)
				return "javascript:;";
			// return 'data:image/svg+xml;charset=utf-8,' + encodeURIComponent($scope.nodeData.svg);
			return maltDataSource.getCallTreeFormatLink($scope.nodeData.nodeId, $scope.filterDepth, 
				$scope.filterHeight, $scope.filterNodeCost, null, $scope.selectedMetric,
				$scope.selector.ratio, "svg");
		};

		$scope.downloadDotLink = function() {
			if(!$scope.nodeData)
				return "javascript:;";
			// return 'data:text/vnd.graphviz;charset=utf-8,' + encodeURIComponent($scope.nodeData.dotCode);
			return maltDataSource.getCallTreeFormatLink($scope.nodeData.nodeId, $scope.filterDepth, 
				$scope.filterHeight, $scope.filterNodeCost, null, $scope.selectedMetric,
				$scope.selector.ratio, "dot");

		};

		$scope.selectNodeEvent = function() {
			loadGraph($scope.contextMenuNodeId, true, true);
		}

		$scope.fitGraphEvent = function() {
			panZoomControls.fit();
			panZoomControls.center();
		}

		$scope.$watch('filterHeight', function() { loadGraph(); });
		$scope.$watch('filterDepth', function() { loadGraph(); });
		$scope.$watch('filterNodeCost', function() { loadGraph(); });
		$scope.$watch('selectedMetric', function() { loadGraph(); });
		$scope.$watch('selector.ratio', function() { loadGraph(); });

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
		maltDataSource.getCallTreeData(-1, $scope.filterDepth, 
			$scope.filterHeight, $scope.filterNodeCost, null, $scope.selectedMetric,
			$scope.selector.ratio, function(data) {
				hideLoader();
				$scope.nodeData = data;
				drawGraph();
				navigateTo($scope.nodeData.nodeId);
				deferredArray[1].resolve();
			}, function() {
				hideLoader();
				$scope.nodeData = {error: {networkError: 'Could not get data from the server.'}};
				deferredArray[1].resolve();				
			});

		$.when(deferredArray).then(function() {
			for (var i = 0; i < $scope.functions.length; i++) {
				if($scope.functions[i].function == $scope.nodeData.function) {
					$scope.$apply(function() {
						$scope.selectedDetails = $scope.functions[i];
					});
					return;
				}
			}
		});

	}]);
}

var maltPageCallTree = new MaltPageCallTree();