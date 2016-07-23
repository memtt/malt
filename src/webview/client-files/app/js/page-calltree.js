/**
 * Provide a call-tree graph which shows all the function calls,
 * the allocations alongwith UI for navigation.
 */
function MaltPageCallTree() 
{
	function getDotCodeForTree(tree, focusedNode)
	{
		var nodes = tree.getNodes(),
			vertices = tree.getEdges();

		return new DotCode()
			.digraph('G', function(d) {
				d.node({
					shape: 'box',
					fontname: 'Courier New',
					fontsize: '12'
				}).edge({
					color: '#868a8f',
					penwidth: 1.5
				});

				for (var i = 0; i < nodes.length; i++) {
					d.node('node' + nodes[i].id, {
						shape: 'record', 
						label: nodes[i].label.trim() + ' | ' + nodes[i].score, 
						style: 'filled',
						color: nodes[i].id == focusedNode ? '#2b2b2b' : 'white',
						penwidth: nodes[i].id == focusedNode ? 3.5 : 1,
						fontcolor: 'white',
						fillcolor: nodes[i].color
					});
				}

				for (var i = 0; i < vertices.length; i++) {
					d.edge('node' + vertices[i].from, 'node' + vertices[i].to);
				}
			})
			.toCode();
	}

	function createSvgGraphForTree(tree, focusedNode) {
		var src = getDotCodeForTree(tree, focusedNode);

		var result = Viz(src, { format:"svg", engine:"dot" });
		var parser = new DOMParser();
		var svg = parser.parseFromString(result, "image/svg+xml");
		svg.documentElement.id = "svggraph";
		document.getElementById("svggraph") && document.getElementById("svggraph").remove();
   		document.getElementById("mynetwork").appendChild(svg.documentElement);

   		svgPanZoom('#svggraph', {
            zoomEnabled: true,
            controlIconsEnabled: false,
            fit: true,
            center: true,
        });
	}

	maltCtrl.controller('malt.page.calltree.ctrl',['$scope','$routeParams','$http', function($scope,$routeParams,$http) {
		var tree = null;

		$scope.file = null;
		$scope.function = null;
		$scope.functions = [];
		$scope.selector = new MaltSelector();
		$scope.totalNodes = 0;
		$scope.visibleNodes = 0;

		$scope.filterHeight = "-1";
		$scope.filterDepth = "3";
		$scope.filterNodeId = 1;
		$scope.filterNodeCost = "1";

		function redrawGraph() {
			if(!tree)
				return;
			tree.resetFilters();
			$scope.totalNodes = tree.getNodes().length;
			tree.filterNodeLine($scope.filterNodeId, parseInt($scope.filterDepth), parseInt($scope.filterHeight));
			tree.filterNodeCost(parseInt($scope.filterNodeCost));
			$scope.visibleNodes = tree.getNodes().length;
			createSvgGraphForTree(tree, $scope.filterNodeId);
		}

		$scope.onFunctionSelectEvent = function(data) {
			var prvFunc = $scope.function, prvFile = $scope.file;
			$scope.function = data.function;
			$scope.file = data.file;
			var node = tree.getNodeByFunctionAndFileName(data.function, data.file, tree);
			if(node == null) {
				if(prvFunc != $scope.function || prvFile != $scope.file)
					alert("Could not find the selected function.");
			} else {
				$scope.filterNodeId = node.id;
				redrawGraph();
			}
		};

		$scope.$watch('filterHeight', redrawGraph);
		$scope.$watch('filterDepth', redrawGraph);
		$scope.$watch('filterNodeCost', redrawGraph);

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

		maltDataSource.getCallStackDataFunc("_start",function(data) {
			$scope.function = '_start';
			$scope.file = '??';

			tree = new CallTreeAdapter(data);
			redrawGraph();
		});

	}]);
}

var maltPageCallTree = new MaltPageCallTree();