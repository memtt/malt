function MaltPageCallTree() 
{
	function SimpleCache() {
		this.cache = [];
		this.id = 1;

		this.exists = function(value) {
			if(this.cache[value] === undefined)
				return false;
			return true;
		}

		this.put = function(value) {
			if(this.exists(value))
				return this.cache[value];
			this.cache[value] = this.id++;
			return this.cache[value];
		}

		this.get = function(value) {
			if(!this.exists(value))
				return false;
			return this.cache[value];
		}

		return this;
	}

	function reduceStat(node,info)
	{
		if(node.info == undefined)
			node.info = jQuery.extend(true, {}, info);
		else
			maltHelper.mergeStackInfoDatas(node.info,info);
	}

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

	function generateNodesAndVertices(tree, level, nodes, vertices, nodeCache, vertCache)
	{
		var identifier = null;
		var currentId = null;


		if(tree.location) {
			// Remove libmalt.so related functions captured in call tree
			if(tree.location.binary.endsWith("libmalt.so"))
				return null;

			// Remove useless nodes
			if(tree.info.alloc.count == 0)
				return null;

			identifier = getIdentifier(tree.location);

			// Add node to node-list if this is a new node
			if(!nodeCache.exists(identifier)) {
				currentId = nodeCache.put(identifier);
				
				nodes.push({
					id: currentId,
					label: tree.location.function,
					level: level,
					score: tree.info.alloc.count
				});
			} else {
				currentId = nodeCache.get(identifier);
			}
		} 

		// Create edge from this node to all its children
		for (var i in tree.childs) {
			if(identifier !=  null) {
				var childId = generateNodesAndVertices(tree.childs[i], level + 1, nodes, vertices, nodeCache, vertCache);
				if(childId != null && !vertCache.exists(currentId + "," + childId)) {
					vertCache.put(currentId + "," + childId);
					vertices.push({
						from: currentId,
						to: childId
					});					
				}
			} else {
				generateNodesAndVertices(tree.childs[i], level + 1, nodes, vertices, nodeCache, vertCache);
			}
		}

		return currentId;
	}

	function generateTreeDataSet(tree) 
	{
		var nodes = [], vertices= [];
		generateNodesAndVertices(tree, 0, nodes, vertices, new SimpleCache(), new SimpleCache());

		return {
			nodes: nodes, 
			edges: vertices
		};
	}

	function generateDotCode(treeDataSet)
	{
		var nodes = treeDataSet.nodes,
			vertices = treeDataSet.edges;
		var nodesArr = [], verticeArr = [];
		for (var i = 0; i < nodes.length; i++) {
			nodesArr.push("node" + nodes[i].id + "[shape=record,label=\"" + nodes[i].label.trim() + " | " + nodes[i].score +"\",style=filled,color=white,fontcolor=white,fillcolor=\"" + nodes[i].color +"\"]");
		}
		for (var i = 0; i < vertices.length; i++) {
			verticeArr.push("node" + vertices[i].from + " -> node" + vertices[i].to);
		}
		return "digraph G {\nnode[shape=\"box\",fontname=\"Courier New\", fontsize=\"12\"];\n" + nodesArr.join(";\n") + ";\n" + verticeArr.join(";\n") + ";\}";
	}

	function addColorCodes(dataset) {
		var nodes = dataset.nodes;
		var max = -1;

		// find max
		for (var i = 0; i < nodes.length; i++) {
			if(nodes[i].score > max) {
				max = nodes[i].score;
			}
		}

		// generate a mapping function from [0-max] onto [#397EBA,#FF9595]
		var colorScale = d3.scale.linear()
			.range(["#397EBA","#ab4141"])
			.domain([0,max]);

		// assign colors
		for (var i = 0; i < nodes.length; i++) {
			nodes[i].color = colorScale(nodes[i].score);
		}
	}

	maltCtrl.controller('malt.page.calltree.ctrl',['$scope','$routeParams','$http', function($scope,$routeParams,$http) {
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

			addColorCodes(dataset);
			var src = generateDotCode(dataset);
			console.log(src);
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