/**
 * Provide a call-tree graph which shows all the function calls,
 * the allocations alongwith UI for navigation.
 */
function MaltPageCallTree() 
{
	function getDotCodeForTree(tree)
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
						color: 'white',
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


	maltCtrl.controller('malt.page.calltree.ctrl',['$scope','$routeParams','$http', function($scope,$routeParams,$http) {
		maltDataSource.getCallStackDataFunc("_start",function(data) {
			var tree = new CallTreeAdapter(data);
			var src = getDotCodeForTree(tree);

			var result = Viz(src, { format:"svg", engine:"dot" });
			var parser = new DOMParser();
			var svg = parser.parseFromString(result, "image/svg+xml");
			svg.documentElement.id = "svggraph";
       		document.getElementById("mynetwork").appendChild(svg.documentElement);

       		svgPanZoom('#svggraph', {
	          zoomEnabled: true,
	          controlIconsEnabled: false,
	          fit: true,
	          center: true,
	        });
		});

	}]);
}

var maltPageCallTree = new MaltPageCallTree();