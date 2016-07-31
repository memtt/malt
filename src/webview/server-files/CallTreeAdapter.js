var d3scale = require("d3-scale");
var extend = require("extend");
var union = require('lodash.union');
var SimpleIdCache = require("./SimpleIdCache.js");
var MaltHelper = require("../client-files/app/js/helper.js");
var CppDeclParser = require("./CppDeclParser.js");

/**
 * Malt helper class object
 * @type {MaltHelper}
 */
var maltHelper = new MaltHelper();

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
			node.info = extend(true, {}, info);
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
		for(var i in data) {
			var call = data[i];
			var cur = tree;
			reduceStat(cur,call.info);
			var tmp = call.stack.reverse();
			for(var x in tmp) {
				var loc = tmp[x];
				if (cur.childs[loc.function] == undefined)
					cur.childs[loc.function] = {childs:{},id:id++,location:loc};
				cur = cur.childs[loc.function];
				reduceStat(cur,call.info);
			}
		}
		
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
					label: CppDeclParser.getShortName(CppDeclParser.parseCppPrototype(tree.location.function)),
					tooltip: tree.location.function,
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

	function convertRgbStringToHex(rgb) {
		var a = rgb.split("(")[1].split(")")[0];
		a = a.split(",");
		var b = a.map(function(x){             //For each array element
		    x = parseInt(x).toString(16);      //Convert to a base16 string
		    return (x.length==1) ? "0"+x : x;  //Add zero if we get only one character
		})
		return "#"+b.join("");
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
		var colorScale = d3scale.scaleLinear()
			.range(["#397EBA","#ab4141"])
			.domain([0,max]);

		// assign colors
		for (var i = 0; i < nodes.length; i++) {
			nodes[i].color = convertRgbStringToHex(colorScale(nodes[i].score));
		}
	}

	/**
	 * Get edges for the Call-tree
	 * @return {array} Array of edges {from, to}
	 */
	this.getEdges = function() {
		return fulltree.edges;
	}

	/**
	 * Get nodes for the call-tree
	 * @return {array} Array of nodes {id, label, level, score}
	 */
	this.getNodes = function() {
		return fulltree.nodes;
	}

	/**
	 * Get a node by function name
	 * @param  {string} func Function name
	 * @return {object}      Node if found, otherwise null
	 */
	this.getNodeByFunctionName = function(func) 
	{
		var nodes = fulltree.nodes;
		for (var i = 0; i < nodes.length; i++) {
			if(nodes[i].data.location.function == func) {
				return nodes[i];
			}
		}
		return null;
	}

	/**
	 * Get a node by its node id
	 * @param  {int} nodeId Node id to search for
	 * @return {object}        Node
	 */
	this.getNodeById = function(nodeId) {
		return fulltree.nodes[nodeId-1];
	}

	/**
	 * Filter a tree to have only decendants of a particular node.
	 * @param  {int} nodeId  Node id of the focal node
	 * @param  {object} nodeSet Set of nodes already in graph
	 * @param  {array} edges   Set of edges already in graph
	 * @param  {int} depth   Depth to limit the tree to. Defaults to unlimited.
	 * @param  {float} costFilter   Mimimum cost for node to be included.
	 */
	function filterDescendantsRecurse(nodeId, nodeSet, edges, depth, costFilter) {
		nodeSet["" + nodeId] = true;

		var currentEdges = fulltree.nodes[nodeId-1].outEdges;
		for (var i = 0; i < currentEdges.length; i++) {
			if(!(("" + currentEdges[i]) in nodeSet)) {
				if(depth !== 0) {
					if(fulltree.nodes[currentEdges[i]-1].score < costFilter)
						return;

					nodeSet["" + currentEdges[i]] = true;

					filterDescendantsRecurse(currentEdges[i], nodeSet, edges, depth - 1, costFilter);
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
	 * @param  {float} costFilter   Mimimum cost for node to be included.
	 */
	function filterAncestorsRecurse(nodeId, nodeSet, edges, height, costFilter) {
		nodeSet["" + nodeId] = true;

		var currentEdges = fulltree.nodes[nodeId-1].inEdges;
		for (var i = 0; i < currentEdges.length; i++) {
			if(!(("" + currentEdges[i]) in nodeSet)) {
				if(fulltree.nodes[currentEdges[i]-1].score < costFilter)
					return;

				if(height !== 0) {
					nodeSet["" + currentEdges[i]] = true;
					filterAncestorsRecurse(currentEdges[i], nodeSet, edges, height - 1, costFilter);
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
	 * @param  {float} costFilter   Mimimum cost for node to be included.
	 */
	this.filterDescendants = function(nodeId, depth, costFilter) {
		var nodeSet = {}, nodeList = [], edgeList = [];
		filterDescendantsRecurse(nodeId, nodeSet, edgeList, depth || -1, costFilter || 0);
		for(var i in nodeSet) {
			nodeList.push(fulltree.nodes[i-1]);
		}
		return {nodes: nodeList, edges: edgeList};
	}

	/**
	 * Filter a tree to have only ancestors of a particular node.
	 * @param  {int} nodeId  Node id of the focal node
	 * @param  {int} height   Height to limit the tree to. Defaults to unlimited.
	 * @param  {float} costFilter   Mimimum cost for node to be included.
	 */
	this.filterAncestors = function(nodeId, height, costFilter) {
		var nodeSet = {}, nodeList = [], edgeList = [];
		filterAncestorsRecurse(nodeId, nodeSet, edgeList, height || -1, costFilter || 0);
		for(var i in nodeSet) {
			nodeList.push(fulltree.nodes[i-1]);
		}
		return {nodes: nodeList, edges: edgeList};
	}

	/**
	 * Filter a tree to have only the ancestors and decendants for a particular node.
	 * @param  {int} nodeId Node id of focal node
	 * @param  {int} depth  Depth to limit to. Defaults to unlimited.
	 * @param  {int} height Height to limit to. Defaults to unlimited. 
	 * @param  {float} costFilterPercentage Minimum cost in percentage for node to be included.
	 */
	this.filterNodeLine = function(nodeId, depth, height, costFilterPercentage) {
		var max = -1;
		for (var i = 0; i < fulltree.nodes.length; i++) {
			if(fulltree.nodes[i].score > max) {
				max = fulltree.nodes[i].score;
			}
		}
		var parentCostFilter = costFilterPercentage/100.0 * max;
		var childrenCostFilter = costFilterPercentage/100.0 * fulltree.nodes[nodeId-1].score;

		var descs = this.filterDescendants(nodeId, depth, childrenCostFilter);
		var ancs = this.filterAncestors(nodeId, height, parentCostFilter);
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
		return {nodes: union(descs.nodes, ancs.nodes), edges: edgeList};
	}

	var fulltree = generateTreeDataSet(buildCallTree(stacktree));
	addColorCodes(fulltree);

	return this;
}

module.exports = CallTreeAdapter;