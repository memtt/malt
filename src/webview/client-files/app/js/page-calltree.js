function MaltPageCallTree() 
{
	/****************************************************/
	function reduceStat(node,info)
	{
		if(node.info == undefined)
			node.info = jQuery.extend(true, {}, info);
		else
			maltHelper.mergeStackInfoDatas(node.info,info);
	}

	/****************************************************/
	function buildCallTree(data)
	{
		var tree = {childs:{},id:null};
		var id = 0;
		data.forEach(function(call) {
			var cur = tree;
			reduceStat(cur,call.info);
			call.stack.reverse().forEach(function(loc) {
				if (cur.childs[loc.function] == undefined)
					cur.childs[loc.function] = {childs:{},id:id++,location:loc};
				cur = cur.childs[loc.function];
				reduceStat(cur,call.info);
			});
		});
		
		return tree;
	}

	function getIdentifier(location) 
	{
		if(!location)
			return null;

		return location.binary + '\\' 
				+ location.file + '\\'
				+ location.function + '\\'
				+ location.line;
	}

	function generateNodesAndVertices(tree, level, nodes, vertices, cache)
	{
		var identifier = null;

		if(tree.location) {
			identifier = getIdentifier(tree.location);

			// if(cache[identifier] === undefined) {
				nodes.push({
					id: tree.id,
					label: tree.location.function,
					level: level,
					score: tree.info.alloc.count
				});

			// 	cache[identifier] = true;	
			// }
		}

		for (var i in tree.childs) {
			if(identifier !=  null) {
				vertices.push({
					from: tree.id,
					to: tree.childs[i].id
				});
			}

			generateNodesAndVertices(tree.childs[i], level + 1, nodes, vertices, cache);
		}
	}

	function generateTreeDataSet(tree) 
	{
		var nodes = [], vertices= [];
		generateNodesAndVertices(tree, 0, nodes, vertices, []);

		return {
			nodes: nodes, 
			edges: vertices
		};
	}

	maltCtrl.controller('malt.page.calltree.ctrl',['$scope','$routeParams','$http', function($scope,$routeParams,$http) {
		$scope.hellotext = "meow";

		var options = {
			nodes: {
				shape: 'box',
				shapeProperties: {
					"borderRadius": 1
				},
				font: {
					"size": 14,
					"face": '"Lucida Console", Monaco, monospace'
				},     
				"shadow": {
					"enabled": true,
					"color": "rgba(0,0,0,0.2)",
					"size": 2,
					"x": 4,
					"y": 4
				}, 
			},
			edges: {
				smooth: {
					type: 'cubicBezier',
					forceDirection: 'vertical',
					roundness: 0.4
				},
				arrows: {
					to: {
						enabled: true,
						scaleFactor: 0.6
					}
				},
			},
			layout: {
				hierarchical: {
					direction: 'UD',
					nodeSpacing: 130
				}
			},
			physics:{
			    enabled: false,
			}

		};



		maltDataSource.getCallStackDataFunc("_start",function(data) {
			var tree = buildCallTree(data);
			console.log("tree", tree);
			var dataset = generateTreeDataSet(tree);
			console.log("nodes", dataset.nodes);
			console.log("vertices", dataset.edges);

			var container = document.getElementById('mynetwork');

			for (var i = 0; i < dataset.nodes.length; i++) {
				dataset.nodes[i].label = dataset.nodes[i].label + '\n';
			}

			var visdata = {nodes: new vis.DataSet(dataset.nodes), edges: new vis.DataSet(dataset.edges)};
			var network = new vis.Network(container, visdata, options);

			// network.on("beforeDrawing", function (ctx) {
			//   // var nodeId = '_start';
			//   // var nodePosition = network.getPositions([nodeId]);
			//   // ctx.strokeStyle = '#A6D5F7';
			//   // ctx.fillStyle = '#294475';
			//   // ctx.circle(nodePosition[nodeId].x, nodePosition[nodeId].y,50);
			//   // ctx.fill();
			//   // ctx.stroke();
			// });
			network.on("afterDrawing", function (ctx) {
				visdata.nodes.forEach(function(d) {
					var nodeId = d.id;
					var nodePosition = network.getPositions([nodeId]);
					ctx.font = "10px Arial";
					ctx.fillText(d.score, nodePosition[nodeId].x, nodePosition[nodeId].y + 10); 
				})
			});
		});


	}]);
}

var maltPageCallTree = new MaltPageCallTree();