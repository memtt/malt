/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/server-files/CallTreeAdapter.js
*-----------------------------------------------------------
*    AUTHOR   : Mehdi Raza Jaffery (CERN) - 2016
*    AUTHOR   : Sébastien Valat - 2022
*    AUTHOR   : Sébastien Valat (INRIA) - 2023 - 2025
***********************************************************/
var d3scale = require("d3-scale");
var extend = require("extend");
var union = require('lodash.union');
var SimpleIdCache = require("./SimpleIdCache.js");
var MaltHelper = require("./helper.js");
var MaltFuncMetrics = require("./func-metrics.js");
var CppDeclParser = require("./CppDeclParser.js");

/**
 * Malt helper class object
 * @type {MaltHelper}
 */
var maltHelper = new MaltHelper();
var maltFuncMetrics = new MaltFuncMetrics();
var allocFuncRegexp = /^((MALT::WrapperPython.*::.*)|(gomp_realloc)|(gomp_malloc)|(gomp_free)|(__gnu_cxx::new_allocator)|(operator new)|(operator delete)|(_Zn[wa])|(g_malloc)|(g_realloc)|(g_free)|(for__get_vm)|(for__free_vm)|([mc]alloc)|(free)|(realloc)|(memalign)|(posix_memalign)|(for_(de)?alloc_allocatable)|(for_(de)?allocate)|(default_malloc)|(default_realloc)|(default_calloc)|(default_free)|(PyDataMem_UserNEW)|(PyArray_NewFromDescr[_a-z]+)|(mmap)|(mremap)|(munmap)|(__pgi_uacc_cuda_alloc))/;

/**
 * An adapter class that encapsulates a stack-tree and exposes
 * it as nodes and edges, suitable for making a calltree graph.
 *
 * @param {object} stacktree Stack tree object
 */
//--PORTED IN C++ VERSION--
function CallTreeAdapter(stacktree)
{

	/**
	 * Creates a cost filter function
	 * @param {float} cost   Cost to be used for comparison
	 * @param {array} minMax An array containing either [min, max] or [min, middle, max]
	 * @param {string} metric Type of metric; determines whether we need > or < comparison
	 */
	//--PORTED IN C++ VERSION--
	function CostFilter(cost, minMax, metric) {
		var comparators = {
			lt: function(left, right) {
				return left <= right;
			},
			gt: function(left, right) {
				return left >= right;
			}
		};

		var minima = minMax[0], maxima = minMax[1], costValue;
		var comparator = maltFuncMetrics.maltMetrics[metric].defaultOrder == 'asc' ? comparators.lt : comparators.gt;

		if(minMax.length == 3) {
			minima = maltFuncMetrics.maltMetrics[metric].defaultOrder == 'asc' ? minMax[1] : minMax[0];
			maxima = maltFuncMetrics.maltMetrics[metric].defaultOrder == 'asc' ? minMax[2] : minMax[1];
		}

		if(maltFuncMetrics.maltMetrics[metric].defaultOrder == 'asc') {
			costValue = maxima - (maxima - minima) * (cost / 100.0);
		} else {
			costValue = (maxima - minima) * (cost / 100.0) + minima;
		}

		return function(value) {
			return comparator(value, costValue);
		}
	}

	/**
	 * Reduce child and parent statistics
	 * @param  {object} node Child node
	 * @param  {object} info Parent node info
	 */
	//--PORTED IN C++ VERSION--
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
	//--PORTED IN C++ VERSION--
	function buildCallTree(data)
	{
		var tree = {childs:{},id:null};
		var id = 0;
		for(var i in data) {
			var call = data[i];
			var cur = tree;
			reduceStat(cur,call.info);
			for (var i = call.stack.length - 1; i >= 0; i--) {
				var loc = call.stack[i];
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
	//--PORTED IN C++ VERSION--
	function getIdentifier(location)
	{
		if(!location)
			return null;

		return location.function;
	}

	/**
	 * Traverse call tree and generate nodes and edges
	 * @param  {object} tree      Hierarchical Call-tree object
	 * @param  {int} level        Current depth
	 * @param  {array} nodes      Nodes array
	 * @param  {array} vertices   Edges array
	 * @param  {SimpleIdCache} nodeCache Cache to track node ids
	 * @param  {SimpleIdCache} vertCache Cache to track edge
	 * @param {array} stack Stack of previous nodes
	 * @return {int}              Id for current tree
	 */
	//--PORTED IN C++ VERSION--
	function generateNodesAndVertices(tree, level, nodes, vertices, nodeCache, vertCache, stack)
	{
		var identifier = null;
		var currentId = null;


		if(tree.location) {
			// [WIP Disabled for now] Remove libmalt.so related functions captured in call tree
			// if(tree.location.binary.endsWith("libmalt.so"))
			// 	return null;

			// Remove useless nodes
			// if(tree.info.alloc.count == 0)
			// 	return null;

			if(allocFuncRegexp.test(tree.location.function))
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
					stats: extend(true, {}, tree.info),
					data: tree,
					outEdges: [],
					inEdges: []
				});
			} else {
				currentId = nodeCache.get(identifier);
				if(stack.indexOf(currentId) == -1)
					maltHelper.mergeStackInfoDatas(nodes[currentId - 1].stats, tree.info);
			}
		}

		// Add current node to stack
		if(currentId)
			stack.push(currentId);

		// Create edge from this node to all its children
		for (var i in tree.childs) {
			if(identifier !=  null) {
				var childId = generateNodesAndVertices(tree.childs[i], level + 1, nodes, vertices, nodeCache, vertCache, stack.slice(0, level));
				if(childId != null && !vertCache.exists(currentId + "," + childId)
					&& stack.slice(0, level-1).indexOf(childId) == -1) {
					// Create edge and copy edge stats
					vertCache.put(currentId + "," + childId);
					var edgeInfo = extend(true, {}, tree.childs[i].info);
					nodes[currentId-1].outEdges.push({id: childId, stats: edgeInfo});
					nodes[childId-1].inEdges.push({id: currentId, stats: edgeInfo});
					vertices.push({
						from: currentId,
						to: childId
					});
				} else if(childId != null && vertCache.exists(currentId + "," + childId)
					&& stack.slice(0, level-1).indexOf(currentId) == -1
					&& stack.slice(0, level-1).indexOf(childId) == -1) {
					// Find existing edge and merge stats
					for (var j = nodes[currentId-1].outEdges.length - 1; j >= 0; j--) {
						if(nodes[currentId-1].outEdges[j].id == childId) {
							maltHelper.mergeStackInfoDatas(nodes[currentId-1].outEdges[j].stats, tree.childs[i].info);
							break;
						}
					}
				}
			} else {
				generateNodesAndVertices(tree.childs[i], level + 1, nodes, vertices, nodeCache, vertCache, stack.slice(0, level));
			}
		}

		return currentId;
	}

	/**
	 * Convert a call tree to a object containing nodes and vertices
	 * @param  {object} tree Call-tree
	 * @return {object}      Contains nodes and edges
	 */
	//--PORTED IN C++ VERSION--
	function generateTreeDataSet(tree)
	{
		var nodes = [], vertices= [];
		generateNodesAndVertices(tree, 0, nodes, vertices, new SimpleIdCache(), new SimpleIdCache(), []);

		return {
			nodes: nodes,
			edges: vertices
		};
	}

	/**
	 * Convert RGB string to HEX color string
	 * @param  {string} rgb RGB color string
	 * @return {string}     HEX color string
	 */
	//--PORTED IN C++ VERSION--
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
	 * Calculates color codes for nodes and edges. based on a metric
	 *
	 * Color is added as the 'color' property on each node and edge.
	 * 'Thickness' is also added to the edges.
	 *
	 * @param {dataset} dataset Call-Tree dataset
	 */
	//--PORTED IN C++ VERSION--
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

		// generate a mapping function from [0-max] onto [1,4]
		var thicknessScale = d3scale.scaleLinear()
			.range([0.8, 8])
			.domain([0,max]);

		// assign colors
		for (var i = 0; i < nodes.length; i++) {
			nodes[i].color = convertRgbStringToHex(colorScale(nodes[i].score));
			for (var j = 0; j < nodes[i].inEdges.length; j++) {
				nodes[i].inEdges[j].color = convertRgbStringToHex(colorScale(nodes[i].inEdges[j].score));
				nodes[i].inEdges[j].thickness = thicknessScale(nodes[i].inEdges[j].score);
			}
			for (var j = 0; j < nodes[i].outEdges.length; j++) {
				nodes[i].outEdges[j].color = convertRgbStringToHex(colorScale(nodes[i].outEdges[j].score));
				nodes[i].outEdges[j].thickness = thicknessScale(nodes[i].outEdges[j].score);
			}
		}
	}

	/**
	 * Add score attribute to nodes
	 * @param {array} nodes  Node list
	 * @param {string} metric Type of metric to use as score
	 * @param {boolean} isRatio Should the score be calculated as percentages?
	 */
	//--PORTED IN C++ VERSION--
	function addScores(nodes, metric, isRatio) {
		var extractValue = maltFuncMetrics.maltMetrics[metric].extractor;
		var formatValue = maltFuncMetrics.maltMetrics[metric].formalter;
		if(isRatio) {
			var max = 0;
			for (var i = 0; i < fulltree.nodes.length; i++) {
				if(fulltree.nodes[i].inEdges.length == 0) {
					var val = maltFuncMetrics.maltMetrics[metric].extractor(fulltree.nodes[i].stats);
					if(maltFuncMetrics.maltMetrics[metric].ref == 'sum') {
						max += val;
					} else {
						max = val > max? val : max;
					}
				}
			}
			for (var i = 0; i < nodes.length; i++) {
				nodes[i].score = extractValue(nodes[i].stats)/max*100.0;
				nodes[i].scoreReadable = Math.round(nodes[i].score*100)/100 + '%';

				for (var j = 0; j < nodes[i].inEdges.length; j++) {
					nodes[i].inEdges[j].score = extractValue(nodes[i].inEdges[j].stats)/max*100.0;
					nodes[i].inEdges[j].scoreReadable = Math.round(nodes[i].inEdges[j].score*100)/100 + '%';
				}
				for (var j = 0; j < nodes[i].outEdges.length; j++) {
					nodes[i].outEdges[j].score = extractValue(nodes[i].outEdges[j].stats)/max*100.0;
					nodes[i].outEdges[j].scoreReadable = Math.round(nodes[i].outEdges[j].score*100)/100 + '%';
				}
			}
		} else {
			for (var i = 0; i < nodes.length; i++) {
				nodes[i].score = extractValue(nodes[i].stats);
				nodes[i].scoreReadable = formatValue(nodes[i].score);

				for (var j = 0; j < nodes[i].inEdges.length; j++) {
					nodes[i].inEdges[j].score = extractValue(nodes[i].inEdges[j].stats);
					nodes[i].inEdges[j].scoreReadable = formatValue(nodes[i].inEdges[j].score);
				}
				for (var j = 0; j < nodes[i].outEdges.length; j++) {
					nodes[i].outEdges[j].score = extractValue(nodes[i].outEdges[j].stats);
					nodes[i].outEdges[j].scoreReadable = formatValue(nodes[i].outEdges[j].score);
				}
			}
		}
	}

	/**
	 * Filter a tree to have only decendants of a particular node.
	 * @param  {int} nodeId  Node id of the focal node
	 * @param  {object} nodeSet Set of nodes already in graph
	 * @param  {array} edges   Set of edges already in graph
	 * @param  {int} depth   Depth to limit the tree to. Defaults to unlimited.
	 * @param  {float} costFilter   Mimimum cost for node to be included.
	 */
	//--PORTED IN C++ VERSION--
	function filterDescendantsRecurse(nodeId, nodeSet, edges, depth, costFilter) {
		nodeSet["" + nodeId] = true;

		var currentEdges = fulltree.nodes[nodeId-1].outEdges;
		for (var i = 0; i < currentEdges.length; i++) {
			if(!(("" + currentEdges[i].id) in nodeSet)) {
				if(depth !== 0) {
					if(!costFilter(fulltree.nodes[currentEdges[i].id-1].score))
						continue;

					nodeSet["" + currentEdges[i].id] = true;

					filterDescendantsRecurse(currentEdges[i].id, nodeSet, edges, depth - 1, costFilter);
				}
			}

			if(("" + currentEdges[i].id) in nodeSet) {
				edges.push({
					from: nodeId,
					to: currentEdges[i].id,
					score: currentEdges[i].scoreReadable,
					color: currentEdges[i].color,
					thickness: currentEdges[i].thickness
				});
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
	//--PORTED IN C++ VERSION--
	function filterAncestorsRecurse(nodeId, nodeSet, edges, height, costFilter) {
		nodeSet["" + nodeId] = true;

		var currentEdges = fulltree.nodes[nodeId-1].inEdges;
		for (var i = 0; i < currentEdges.length; i++) {
			if(!(("" + currentEdges[i].id) in nodeSet)) {
				if(!costFilter(fulltree.nodes[currentEdges[i].id-1].score))
					continue;

				if(height !== 0) {
					nodeSet["" + currentEdges[i].id] = true;
					filterAncestorsRecurse(currentEdges[i].id, nodeSet, edges, height - 1, costFilter);
				}

			}

			if(("" + currentEdges[i].id) in nodeSet) {
				edges.push({
					from: currentEdges[i].id,
					to: nodeId,
					score: currentEdges[i].scoreReadable,
					color: currentEdges[i].color,
					thickness: currentEdges[i].thickness
				});
			}
		}
	}

	/**
	 * Filter a tree to have only decendants of a particular node.
	 * @param  {int} nodeId  Node id of the focal node
	 * @param  {int} depth   Depth to limit the tree to. Defaults to unlimited.
	 * @param  {float} costFilter   Mimimum cost for node to be included.
	 * @return {object}                      A tree object containing 'nodes' and 'edges'.
	 */
	//--PORTED IN C++ VERSION--
	function filterDescendants(nodeId, depth, costFilter) {
		var nodeSet = {}, nodeList = [], edgeList = [];
		filterDescendantsRecurse(nodeId, nodeSet, edgeList, depth || -1, costFilter);
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
	 * @return {object}                      A tree object containing 'nodes' and 'edges'.
	 */
	//--PORTED IN C++ VERSION--
	function filterAncestors (nodeId, height, costFilter) {
		var nodeSet = {}, nodeList = [], edgeList = [];
		filterAncestorsRecurse(nodeId, nodeSet, edgeList, height || -1, costFilter);
		for(var i in nodeSet) {
			nodeList.push(fulltree.nodes[i-1]);
		}
		return {nodes: nodeList, edges: edgeList};
	}

	/**
	 * Get edges for the Call-tree
	 * @return {array} Array of edges {from, to}
	 */
	//--PORTED IN C++ VERSION--
	this.getEdges = function() {
		return fulltree.edges;
	}

	/**
	 * Get nodes for the call-tree
	 * @return {array} Array of nodes {id, label, level, score}
	 */
	//--PORTED IN C++ VERSION--
	this.getNodes = function() {
		return fulltree.nodes;
	}

	/**
	 * Get a node by function name
	 * @param  {string} func Function name
	 * @return {object}      Node if found, otherwise null
	 */
	//--PORTED IN C++ VERSION--
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
	//--PORTED IN C++ VERSION--
	this.getNodeById = function(nodeId) {
		return fulltree.nodes[nodeId-1];
	}

	/**
	 * Filter a tree to have only the ancestors and decendants for a particular node.
	 * @param  {int} nodeId Node id of focal node
	 * @param  {int} depth  Depth to limit to. Defaults to unlimited.
	 * @param  {int} height Height to limit to. Defaults to unlimited.
	 * @param  {float} costFilterPercentage Minimum cost in percentage for node to be included.
	 * @param  {string} metric               Type of metric to use as score.
	 * @param {boolean} isRatio Should the node scores be calculated as percentages?
	 * @return {object}                      A tree object containing 'nodes' and 'edges'.
	 */
	//--PORTED IN C++ VERSION--
	this.filterNodeLine = function(nodeId, depth, height, costFilterPercentage, metric, isRatio) {
		addScores(fulltree.nodes, metric, isRatio);
		addColorCodes(fulltree);

		var max = -1;
		for (var i = 0; i < fulltree.nodes.length; i++) {
			if(fulltree.nodes[i].score > max) {
				max = fulltree.nodes[i].score;
			}
		}
		var min = max + 1;
		for (var i = 0; i < fulltree.nodes.length; i++) {
			if(fulltree.nodes[i].score < min) {
				min = fulltree.nodes[i].score;
			}
		}

		var childrenCostFilter = new CostFilter(costFilterPercentage, [min, fulltree.nodes[nodeId-1].score, max], metric);
		var parentCostFilter = new CostFilter(costFilterPercentage, [min, max], metric);
		var descs = filterDescendants(nodeId, depth, childrenCostFilter);
		var ancs = filterAncestors(nodeId, height, parentCostFilter);

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

	/**
	 * Filter a tree to get all root nodes plus their descendants
	 * @param  {int}    depth                Depth to limit to. Defaults to unlimited.
	 * @param  {int}    costFilterPercentage Minimum cost in percentage for node to be included.
	 * @param  {string} metric               Type of metric to use as score.
	 * @param {boolean} isRatio Should the node scores be calculated as percentages?
	 * @return {object}                      A tree object containing 'nodes' and 'edges'.
	 */
	//--PORTED IN C++ VERSION--
	this.filterRootLines = function(depth, costFilterPercentage, metric, isRatio) {
		addScores(fulltree.nodes, metric, isRatio);
		addColorCodes(fulltree);

		var max = -1;
		for (var i = 0; i < fulltree.nodes.length; i++) {
			if(fulltree.nodes[i].score > max) {
				max = fulltree.nodes[i].score;
			}
		}
		var min = max + 1;
		for (var i = 0; i < fulltree.nodes.length; i++) {
			if(fulltree.nodes[i].score < min) {
				min = fulltree.nodes[i].score;
			}
		}

		var nodeSet = {}, edgeList = [];
		for (var i = 0; i < fulltree.nodes.length; i++) {
			if(fulltree.nodes[i].inEdges.length == 0) {
				var childrenCostFilter = new CostFilter(costFilterPercentage, [min, fulltree.nodes[i].score, max], metric);
				filterDescendantsRecurse(fulltree.nodes[i].id, nodeSet, edgeList, depth, childrenCostFilter);
			}
		}

		var edgeSet = {};
		var edges = [];
		var nodes = [];
		for (var i = 0; i < edgeList.length; i++) {
			edgeSet[edgeList[i].from + ',' + edgeList[i].to] = edgeList[i];
		}
		for(var i in edgeSet) {
			edges.push(edgeSet[i]);
		}
		for(var i in nodeSet) {
			nodes.push(fulltree.nodes[i-1]);
		}

		return {nodes: nodes, edges: edges};
	}

	// console.time("buildCallTree");
	var calltree = buildCallTree(stacktree);
	// console.timeEnd("buildCallTree");

	// console.time("generateTreeDataSet");
	var fulltree = generateTreeDataSet(calltree);
	// console.timeEnd("generateTreeDataSet");

	// console.time("addColorCodes");
	// console.timeEnd("addColorCodes");

	return this;
}

module.exports = CallTreeAdapter;
