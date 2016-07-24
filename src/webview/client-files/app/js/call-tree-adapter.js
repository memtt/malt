/**
 * An adapter class that encapsulates a stack-tree and exposes
 * it as nodes and edges, suitable for making a calltree graph.
 * 
 * @param {object} stacktree Stack tree object
 */
function CallTreeAdapter(stacktree)
{
	/**
	 * Reduce child and parent statistics
	 * @param  {object} node Child node
	 * @param  {object} info Parent node info
	 */
	function reduceStat(node,info)
	{
		if(node.info == undefined)
			node.info = jQuery.extend(true, {}, info);
		else
			maltHelper.mergeStackInfoDatas(node.info,info);
	}

	/**
	 * Builds a hierarchical call tree from a stack tree
	 * @param  {object} data Stack tree data
	 * @return {object}      Call tree
	 */
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

	/**
	 * Creates identifier from a location object for a function
	 * @param  {object} location Location object for a function
	 * @return {string}          Identifier
	 */
	function getIdentifier(location) 
	{
		if(!location)
			return null;

		return location.binary + '\\' 
				+ location.file + '\\'
				+ location.function + '\\'
				+ location.line;
	}

	var tagsToReplace = {
	    '&': '&amp;',
	    '<': '&lt;',
	    '>': '&gt;'
	};

	function replaceTag(tag) {
	    return tagsToReplace[tag] || tag;
	}

	function safe_tags_replace(str) {
	    return str.replace(/[&<>]/g, replaceTag);
	}

	/**
	 * Traverse call tree and generate nodes and edges
	 * @param  {object} tree      Hierarchical Call-tree object
	 * @param  {int} level        Current depth
	 * @param  {array} nodes      Nodes array
	 * @param  {array} vertices   Edges array
	 * @param  {SimpleIdCache} nodeCache Cache to track node ids
	 * @param  {SimpleIdCache} vertCache Cache to track edge
	 * @return {int}              Id for current tree
	 */
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
					label: safe_tags_replace(tree.location.function),
					level: level,
					score: tree.info.alloc.count,
					data: tree,
					outEdges: [],
					inEdges: []
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
					nodes[currentId-1].outEdges.push(childId);
					nodes[childId-1].inEdges.push(currentId);
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

	/**
	 * Convert a call tree to a object containing nodes and vertices
	 * @param  {object} tree Call-tree
	 * @return {object}      Contains nodes and edges
	 */
	function generateTreeDataSet(tree) 
	{
		var nodes = [], vertices= [];
		generateNodesAndVertices(tree, 0, nodes, vertices, new SimpleIdCache(), new SimpleIdCache());

		return {
			nodes: nodes, 
			edges: vertices
		};
	}

	/**
	 * Calculates color codes for nodes based on a metric
	 *
	 * Color is added as the 'color' property on each node
	 * 
	 * @param {dataset} dataset Call-Tree dataset
	 */
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

	/**
	 * Get edges for the Call-tree
	 * @return {array} Array of edges {from, to}
	 */
	this.getEdges = function() {
		return tree.edges;
	}

	/**
	 * Get nodes for the call-tree
	 * @return {array} Array of nodes {id, label, level, score}
	 */
	this.getNodes = function() {
		return tree.nodes;
	}

	/**
	 * Get a node by function and file name
	 * @param  {string} func Function name
	 * @param  {string} file Filename
	 * @return {object}      Node if found, otherwise null
	 */
	this.getNodeByFunctionAndFileName = function(func, file) 
	{
		var nodes = fulltree.nodes;
		for (var i = 0; i < nodes.length; i++) {
			if(nodes[i].data.location.function == func && nodes[i].data.location.file == file) {
				return nodes[i];
			}
		}
		return null;
	}

	/**
	 * Filter a tree to have only decendants of a particular node.
	 * @param  {int} nodeId  Node id of the focal node
	 * @param  {object} nodeSet Set of nodes already in graph
	 * @param  {array} edges   Set of edges already in graph
	 * @param  {int} depth   Depth to limit the tree to. Defaults to unlimited.
	 */
	function filterDescendantsRecurse(nodeId, nodeSet, edges, depth) {
		nodeSet["" + nodeId] = true;

		var currentEdges = fulltree.nodes[nodeId-1].outEdges;
		for (var i = 0; i < currentEdges.length; i++) {
			if(!(("" + currentEdges[i]) in nodeSet)) {
				if(depth !== 0) {
					nodeSet["" + currentEdges[i]] = true;

					filterDescendantsRecurse(currentEdges[i], nodeSet, edges, depth - 1);
				}
			}
			
			if(("" + currentEdges[i]) in nodeSet) {
				edges.push({from: nodeId, to: currentEdges[i] });
			}
		}
	}

	/**
	 * Filter a tree to have only ancestors of a particular node.
	 * @param  {int} nodeId  Node id of the focal node
	 * @param  {object} nodeSet Set of nodes already in graph
	 * @param  {array} edges   Set of edges already in graph
	 * @param  {int} height   height to limit the tree to. Defaults to unlimited.
	 */
	function filterAncestorsRecurse(nodeId, nodeSet, edges, height) {
		nodeSet["" + nodeId] = true;
		var currentEdges = fulltree.nodes[nodeId-1].inEdges;
		for (var i = 0; i < currentEdges.length; i++) {
			if(!(("" + currentEdges[i]) in nodeSet)) {
				if(height !== 0) {
					nodeSet["" + currentEdges[i]] = true;
					filterAncestorsRecurse(currentEdges[i], nodeSet, edges, height - 1);
				}

			}

			if(("" + currentEdges[i]) in nodeSet) {
				edges.push({from: currentEdges[i], to: nodeId});
			}
		}
	}

	/**
	 * Filter a tree to have only decendants of a particular node.
	 * @param  {int} nodeId  Node id of the focal node
	 * @param  {int} depth   Depth to limit the tree to. Defaults to unlimited.
	 */
	this.filterDescendants = function(nodeId, depth) {
		var nodeSet = {}, nodeList = [], edgeList = [];
		filterDescendantsRecurse(nodeId, nodeSet, edgeList, depth || -1);
		for(var i in nodeSet) {
			nodeList.push(fulltree.nodes[i-1]);
		}
		tree = {nodes: nodeList, edges: edgeList};
	}

	/**
	 * Filter a tree to have only ancestors of a particular node.
	 * @param  {int} nodeId  Node id of the focal node
	 * @param  {int} height   Height to limit the tree to. Defaults to unlimited.
	 */
	this.filterAncestors = function(nodeId, height) {
		var nodeSet = {}, nodeList = [], edgeList = [];
		filterAncestorsRecurse(nodeId, nodeSet, edgeList, height || -1);
		for(var i in nodeSet) {
			nodeList.push(fulltree.nodes[i-1]);
		}
		tree = {nodes: nodeList, edges: edgeList};
	}

	/**
	 * Filter a tree to have only the ancestors and decendants for a particular node.
	 * @param  {int} nodeId Node id of focal node
	 * @param  {int} depth  Depth to limit to. Defaults to unlimited.
	 * @param  {int} height Height to limit to. Defaults to unlimited. 
	 */
	this.filterNodeLine = function(nodeId, depth, height) {
		this.filterDescendants(nodeId, depth);
		var descs = tree;
		this.resetFilters();
		this.filterAncestors(nodeId, height);
		var ancs = tree;
		var edgeSet = {};
		for (var i = 0; i < descs.edges.length; i++) {
			edgeSet[descs.edges[i].from + ',' + descs.edges[i].to] = descs.edges[i];
		}
		for (var i = 0; i < ancs.edges.length; i++) {
			edgeSet[ancs.edges[i].from + ',' + ancs.edges[i].to] = ancs.edges[i];
		}
		var edgeList = [];
		for(var i in edgeSet) {
			edgeList.push(edgeSet[i]);
		}
		tree = {nodes: descs.nodes.concat(ancs.nodes), edges: edgeList};
	}

	/**
	 * Filter a tree to have only nodes with score greater than the given cost.
	 * @param  {number} costPercentage Minimum cost
	 */
	this.filterNodeCost = function(costPercentage) {
		var max = -1;
		var cost = costPercentage/100.0;
		var nodeSet = {};

		// find max
		for (var i = 0; i < tree.nodes.length; i++) {
			if(tree.nodes[i].score > max) {
				max = tree.nodes[i].score;
			}
		}

		for (var i = 0; i < tree.nodes.length; i++) {
			if(tree.nodes[i].score/max < cost) {
				nodeSet["" + tree.nodes[i].id] = true;
				tree.nodes.splice(i, 1);
				i--;
			}
		}

		for (var i = 0; i < tree.edges.length; i++) {
			if(("" + tree.edges[i].from) in nodeSet ||
				("" + tree.edges[i].to) in nodeSet) {
				tree.edges.splice(i, 1);
				i--;
			}
		}
	}

	/**
	 * Remove filters and reset tree to full view.
	 */
	this.resetFilters = function() {
		tree = fulltree;
		// tree =  JSON.parse(JSON.stringify(fulltree));;
	}

	var tree = generateTreeDataSet(buildCallTree(stacktree));
	var fulltree = tree;
	addColorCodes(tree);
	console.log(tree);

	return this;
}