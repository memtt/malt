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

	var tree = generateTreeDataSet(buildCallTree(stacktree));
	var fulltree = tree;
	addColorCodes(tree);
	console.log(tree);
	
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

	function filterDescendantsRecurse(nodeId, nodeSet, edges, depth) {
		nodeSet["" + nodeId] = true;

		for (var i = 0; i < fulltree.edges.length; i++) {
			if(fulltree.edges[i].from == nodeId) {
				// console.log(fulltree.edges[i]);
				if(!(("" + fulltree.edges[i].to) in nodeSet)) {
					if(depth !== 0) {
						nodeSet["" + fulltree.edges[i].to] = true;
						// console.log("entering node", fulltree.edges[i].to)

						filterDescendantsRecurse(fulltree.edges[i].to, nodeSet, edges, depth - 1);
					}
				}
				
				if(("" + fulltree.edges[i].to) in nodeSet) {
					edges.push(fulltree.edges[i]);
				}
			}
		}
	}

	function filterAncestorsRecurse(nodeId, nodeSet, edges, height) {
		nodeSet["" + nodeId] = true;
		for (var i = 0; i < fulltree.edges.length; i++) {
			if(fulltree.edges[i].to == nodeId) {
				// console.log(fulltree.edges[i]);
				if(!(("" + fulltree.edges[i].from) in nodeSet)) {
					if(height !== 0) {
						nodeSet["" + fulltree.edges[i].from] = true;
						// console.log("entering node", fulltree.edges[i].to)
						filterAncestorsRecurse(fulltree.edges[i].from, nodeSet, edges, height - 1);
					}

				}

				if(("" + fulltree.edges[i].from) in nodeSet) {
					edges.push(fulltree.edges[i]);
				}
			}
		}
	}

	this.filterDescendants = function(nodeId, depth) {
		var nodeSet = {}, nodeList = [], edgeList = [];
		filterDescendantsRecurse(nodeId, nodeSet, edgeList, depth || -1);
		// console.log(nodeSet)
		for(var i in nodeSet) {
			nodeList.push(fulltree.nodes[i-1]);
		}
		tree = {nodes: nodeList, edges: edgeList};
	}

	this.filterAncestors = function(nodeId, height) {
		var nodeSet = {}, nodeList = [], edgeList = [];
		filterAncestorsRecurse(nodeId, nodeSet, edgeList, height || -1);
		// console.log(nodeSet)
		for(var i in nodeSet) {
			nodeList.push(fulltree.nodes[i-1]);
		}
		tree = {nodes: nodeList, edges: edgeList};
	}

	this.filterNodeLine = function(nodeId, depth, height) {
		this.filterDescendants(nodeId, depth);
		var descs = tree;
		this.resetFilters();
		this.filterAncestors(nodeId, height);
		var ancs = tree;
		tree = {nodes: descs.nodes.concat(ancs.nodes), edges: descs.edges.concat(ancs.edges)};
		console.log(tree);
	}

	this.resetFilters = function() {
		tree = fulltree;
	}

	// console.log(this.filterDescendants(17));

	return this;
}