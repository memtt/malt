/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/server-files/CallTreeAdapter.js
*-----------------------------------------------------------
*    AUTHOR   : Mehdi Raza Jaffery (CERN) - 2016
*    AUTHOR   : Sébastien Valat - 2022 - 2024
*    AUTHOR   : Sébastien Valat (INRIA) - 2023
***********************************************************/

/********************** AUTHOR NOTE ************************
This file is originally written in pure JS by 
 - Mehdi Raza Jaffery (CERN) - 2016
It has been rewritten in C++ by
 - Sébastien Valat (INRIA) - 2025
/**********************************************************/

/**********************************************************/
#include "ExtractorHelpers.hpp"
#include "CppDeclParser.hpp"
#include "CallTreeAdapter.hpp"

namespace MALTReader
{

/**********************************************************/
CallTreeNode::CallTreeNode(size_t id, std::string label, std::string tooltip, size_t level, MALTFormat::StackInfos stats, const FullTreeNode & treeNode)
	:id(id)
	,label(label)
	,tooltip(tooltip)
	,level(level)
	,stats(stats)
	,treeNode(treeNode)
{

}

/**********************************************************/
CostFilter::CostFilter(float cost, float min, float max, const MaltMetric & metric)
{
	this->setup(cost, min, max, metric);
}

/**********************************************************/
CostFilter::CostFilter(float cost, float min, float middle, float max, const MaltMetric & metric)
{
	if (metric.defaultOrder == SORT_ASC) {
		this->setup(cost, middle, max, metric);
	} else {
		this->setup(cost, min, middle, metric);
	}
}

/**********************************************************/
void CostFilter::setup(float cost, float min, float max, const MaltMetric & metric)
{
	//calc cost value
	if(metric.defaultOrder == SORT_ASC) {
		this->costValue = max - (max - min) * (cost / 100.0);
	} else {
		this->costValue = (max - min) * (cost / 100.0) + min;
	}

	//setup
	if  (metric.defaultOrder == SORT_ASC)
		this->greaterThan = false;
	else
		this->greaterThan = true;
}

/**********************************************************/
bool CostFilter::check(float value) const
{
	if (this->greaterThan)
		return value >= this->costValue;
	else
		return value <= this->costValue;
}

/**********************************************************/
CallTreeAdapter::CallTreeAdapter(const Extractor & extractor)
	:extractor(extractor)
{
	//build calltree
	FullTreeNode calltree = extractor.getFullTree();

	//build graph
	TreeSet treeSet = this->generateTreeDataSet(calltree);
}

/**********************************************************/
CallTreeAdapter::~CallTreeAdapter(void)
{

}

static ssize_t getIndexOf(const std::vector<size_t> & stack, size_t levelMax, size_t id)
{
	//check
	assert(levelMax <= stack.size());

	//loop
	for (size_t i = 0 ; i < levelMax ; i++)
		if (stack[i] == id)
			return i;

	//not found
	return -1;
}

/**********************************************************/
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
size_t CallTreeAdapter::generateNodesAndVertices(const FullTreeNode & treeNode, size_t level, std::vector<CallTreeNode> & nodes, std::vector<CallTreeEdge> & vertices, SimpleIdCache & nodeCache, SimpleIdCache & vertCache, std::vector<size_t> stack)
{
	//vars
	std::string identifier;
	size_t currentId = CACHE_ID_NULL;

	if(treeNode.location != nullptr) {
		// [WIP Disabled for now] Remove libmalt.so related functions captured in call tree
		// if(tree.location.binary.endsWith("libmalt.so"))
		// 	return null;

		// Remove useless nodes
		// if(tree.info.alloc.count == 0)
		// 	return null;

		//skip alloc functions
		if (ExtractorHelpers::isAllocFunction(*treeNode.location->function))
			return CACHE_ID_NULL;

		//get identifier
		identifier = *treeNode.location->function;

		// Add node to node-list if this is a new node
		if(!nodeCache.exists(identifier)) {
			currentId = nodeCache.put(identifier);
			const std::string label = CppDeclParser::getShortName(CppDeclParser::parseCppPrototype(*treeNode.location->function));
			const std::string tooltip = *treeNode.location->function;
			nodes.emplace_back(currentId, label, tooltip, level, *treeNode.infos, treeNode);
		} else {
			currentId = nodeCache.get(identifier);
			if(getIndexOf(stack, stack.size(), currentId) == -1)
				nodes[currentId - 1].stats.merge(*treeNode.infos);
		}
	}

	// Add current node to stack
	if(currentId)
		stack.push_back(currentId);

	// Create edge from this node to all its children
	for (const auto & it : treeNode.child) {
		if(identifier.empty() == false) {
			//build child
			stack.push_back(CACHE_ID_NULL);
			size_t childId = generateNodesAndVertices(it.second, level + 1, nodes, vertices, nodeCache, vertCache, stack);
			stack.pop_back();

			//build vertice name
			char vertName[1024];
			snprintf(vertName, sizeof(vertName), "%zu-%zu", currentId, childId);

			//check
			if(childId != CACHE_ID_NULL && !vertCache.exists(vertName) && getIndexOf(stack, level-1, childId) == -1) {
				// Create edge and copy edge stats
				vertCache.put(vertName);

				//update links
				nodes[currentId-1].outEdges.emplace_back(childId, *it.second.infos);
				nodes[childId-1].inEdges.emplace_back(currentId, *it.second.infos);

				//add vertice
				vertices.emplace_back(currentId, childId);
			} else if(childId != CACHE_ID_NULL && vertCache.exists(vertName) && getIndexOf(stack, level-1, currentId) == -1 && getIndexOf(stack, level-1, childId) == -1) {
				// Find existing edge and merge stats
				for (ssize_t j = nodes[currentId-1].outEdges.size() - 1; j >= 0; j--) {
					if(nodes[currentId-1].outEdges[j].id == childId) {
						nodes[currentId-1].outEdges[j].stats.merge(*it.second.infos);
						break;
					}
				}
			}
		} else {
			stack.push_back(CACHE_ID_NULL);
			generateNodesAndVertices(it.second, level + 1, nodes, vertices, nodeCache, vertCache, stack);
			stack.pop_back();
		}
	}

	for (const auto & it : treeNode.child) {
		if(identifier.empty() == false) {
			//go child
			stack.push_back(CACHE_ID_NULL);
			size_t childId = generateNodesAndVertices(it.second, level + 1, nodes, vertices, nodeCache, vertCache, stack);
			stack.pop_back();

			//build vertice name
			char vertName[1024];
			snprintf(vertName, sizeof(vertName), "%zu-%zu", currentId, childId);

			//build vertices
			if(childId != CACHE_ID_NULL && !vertCache.exists(vertName) && getIndexOf(stack, level-1, childId) == -1) {
				// Create edge and copy edge stats
				vertCache.put(vertName);

				//fill links
				nodes[currentId-1].outEdges.emplace_back(childId, *it.second.infos);
				nodes[childId-1].inEdges.emplace_back(currentId, *it.second.infos);

				//create vertice
				vertices.emplace_back(currentId, childId);
			} else if(childId != CACHE_ID_NULL && vertCache.exists(vertName) && getIndexOf(stack, level-1, currentId) == -1 && getIndexOf(stack, level-1, childId) == -1) {
				// Find existing edge and merge stats
				for (ssize_t j = nodes[currentId-1].outEdges.size() - 1; j >= 0; j--) {
					if(nodes[currentId-1].outEdges[j].id == childId) {
						nodes[currentId-1].outEdges[j].stats.merge(*it.second.infos);
						break;
					}
				}
			}
		} else {
			stack.push_back(CACHE_ID_NULL);
			generateNodesAndVertices(it.second, level + 1, nodes, vertices, nodeCache, vertCache, stack);
			stack.pop_back();
		}
	}

	return currentId;
}

/**********************************************************/
/**
 * Convert a call tree to a object containing nodes and vertices
 * @param  {object} tree Call-tree
 * @return {object}      Contains nodes and edges
 */
TreeSet CallTreeAdapter::generateTreeDataSet(FullTreeNode & treeNode)
{
	TreeSet set;
	std::vector<size_t> stack;
	SimpleIdCache nodeIdCache;
	SimpleIdCache vertIdCache;
	generateNodesAndVertices(treeNode, 0, set.nodes, set.vertices, nodeIdCache, vertIdCache, stack);
	return set;
}

}



// 	/**
// 	 * Convert RGB string to HEX color string
// 	 * @param  {string} rgb RGB color string
// 	 * @return {string}     HEX color string
// 	 */
// 	function convertRgbStringToHex(rgb) {
// 		var a = rgb.split("(")[1].split(")")[0];
// 		a = a.split(",");
// 		var b = a.map(function(x){             //For each array element
// 		    x = parseInt(x).toString(16);      //Convert to a base16 string
// 		    return (x.length==1) ? "0"+x : x;  //Add zero if we get only one character
// 		})
// 		return "#"+b.join("");
// 	}

// 	/**
// 	 * Calculates color codes for nodes and edges. based on a metric
// 	 *
// 	 * Color is added as the 'color' property on each node and edge.
// 	 * 'Thickness' is also added to the edges.
// 	 *
// 	 * @param {dataset} dataset Call-Tree dataset
// 	 */
// 	function addColorCodes(dataset) {
// 		var nodes = dataset.nodes;
// 		var max = -1;

// 		// find max
// 		for (var i = 0; i < nodes.length; i++) {
// 			if(nodes[i].score > max) {
// 				max = nodes[i].score;
// 			}
// 		}

// 		// generate a mapping function from [0-max] onto [#397EBA,#FF9595]
// 		var colorScale = d3scale.scaleLinear()
// 			.range(["#397EBA","#ab4141"])
// 			.domain([0,max]);

// 		// generate a mapping function from [0-max] onto [1,4]
// 		var thicknessScale = d3scale.scaleLinear()
// 			.range([0.8, 8])
// 			.domain([0,max]);

// 		// assign colors
// 		for (var i = 0; i < nodes.length; i++) {
// 			nodes[i].color = convertRgbStringToHex(colorScale(nodes[i].score));
// 			for (var j = 0; j < nodes[i].inEdges.length; j++) {
// 				nodes[i].inEdges[j].color = convertRgbStringToHex(colorScale(nodes[i].inEdges[j].score));
// 				nodes[i].inEdges[j].thickness = thicknessScale(nodes[i].inEdges[j].score);
// 			}
// 			for (var j = 0; j < nodes[i].outEdges.length; j++) {
// 				nodes[i].outEdges[j].color = convertRgbStringToHex(colorScale(nodes[i].outEdges[j].score));
// 				nodes[i].outEdges[j].thickness = thicknessScale(nodes[i].outEdges[j].score);
// 			}
// 		}
// 	}

// 	/**
// 	 * Add score attribute to nodes
// 	 * @param {array} nodes  Node list
// 	 * @param {string} metric Type of metric to use as score
// 	 * @param {boolean} isRatio Should the score be calculated as percentages?
// 	 */
// 	function addScores(nodes, metric, isRatio) {
// 		var extractValue = maltFuncMetrics.maltMetrics[metric].extractor;
// 		var formatValue = maltFuncMetrics.maltMetrics[metric].formalter;
// 		if(isRatio) {
// 			var max = 0;
// 			for (var i = 0; i < fulltree.nodes.length; i++) {
// 				if(fulltree.nodes[i].inEdges.length == 0) {
// 					var val = maltFuncMetrics.maltMetrics[metric].extractor(fulltree.nodes[i].stats);
// 					if(maltFuncMetrics.maltMetrics[metric].ref == 'sum') {
// 						max += val;
// 					} else {
// 						max = val > max? val : max;
// 					}
// 				}
// 			}
// 			for (var i = 0; i < nodes.length; i++) {
// 				nodes[i].score = extractValue(nodes[i].stats)/max*100.0;
// 				nodes[i].scoreReadable = Math.round(nodes[i].score*100)/100 + '%';

// 				for (var j = 0; j < nodes[i].inEdges.length; j++) {
// 					nodes[i].inEdges[j].score = extractValue(nodes[i].inEdges[j].stats)/max*100.0;
// 					nodes[i].inEdges[j].scoreReadable = Math.round(nodes[i].inEdges[j].score*100)/100 + '%';
// 				}
// 				for (var j = 0; j < nodes[i].outEdges.length; j++) {
// 					nodes[i].outEdges[j].score = extractValue(nodes[i].outEdges[j].stats)/max*100.0;
// 					nodes[i].outEdges[j].scoreReadable = Math.round(nodes[i].outEdges[j].score*100)/100 + '%';
// 				}
// 			}
// 		} else {
// 			for (var i = 0; i < nodes.length; i++) {
// 				nodes[i].score = extractValue(nodes[i].stats);
// 				nodes[i].scoreReadable = formatValue(nodes[i].score);

// 				for (var j = 0; j < nodes[i].inEdges.length; j++) {
// 					nodes[i].inEdges[j].score = extractValue(nodes[i].inEdges[j].stats);
// 					nodes[i].inEdges[j].scoreReadable = formatValue(nodes[i].inEdges[j].score);
// 				}
// 				for (var j = 0; j < nodes[i].outEdges.length; j++) {
// 					nodes[i].outEdges[j].score = extractValue(nodes[i].outEdges[j].stats);
// 					nodes[i].outEdges[j].scoreReadable = formatValue(nodes[i].outEdges[j].score);
// 				}
// 			}
// 		}
// 	}

// 	/**
// 	 * Filter a tree to have only decendants of a particular node.
// 	 * @param  {int} nodeId  Node id of the focal node
// 	 * @param  {object} nodeSet Set of nodes already in graph
// 	 * @param  {array} edges   Set of edges already in graph
// 	 * @param  {int} depth   Depth to limit the tree to. Defaults to unlimited.
// 	 * @param  {float} costFilter   Mimimum cost for node to be included.
// 	 */
// 	function filterDescendantsRecurse(nodeId, nodeSet, edges, depth, costFilter) {
// 		nodeSet["" + nodeId] = true;

// 		var currentEdges = fulltree.nodes[nodeId-1].outEdges;
// 		for (var i = 0; i < currentEdges.length; i++) {
// 			if(!(("" + currentEdges[i].id) in nodeSet)) {
// 				if(depth !== 0) {
// 					if(!costFilter(fulltree.nodes[currentEdges[i].id-1].score))
// 						continue;

// 					nodeSet["" + currentEdges[i].id] = true;

// 					filterDescendantsRecurse(currentEdges[i].id, nodeSet, edges, depth - 1, costFilter);
// 				}
// 			}

// 			if(("" + currentEdges[i].id) in nodeSet) {
// 				edges.push({
// 					from: nodeId,
// 					to: currentEdges[i].id,
// 					score: currentEdges[i].scoreReadable,
// 					color: currentEdges[i].color,
// 					thickness: currentEdges[i].thickness
// 				});
// 			}
// 		}
// 	}

// 	/**
// 	 * Filter a tree to have only ancestors of a particular node.
// 	 * @param  {int} nodeId  Node id of the focal node
// 	 * @param  {object} nodeSet Set of nodes already in graph
// 	 * @param  {array} edges   Set of edges already in graph
// 	 * @param  {int} height   height to limit the tree to. Defaults to unlimited.
// 	 * @param  {float} costFilter   Mimimum cost for node to be included.
// 	 */
// 	function filterAncestorsRecurse(nodeId, nodeSet, edges, height, costFilter) {
// 		nodeSet["" + nodeId] = true;

// 		var currentEdges = fulltree.nodes[nodeId-1].inEdges;
// 		for (var i = 0; i < currentEdges.length; i++) {
// 			if(!(("" + currentEdges[i].id) in nodeSet)) {
// 				if(!costFilter(fulltree.nodes[currentEdges[i].id-1].score))
// 					continue;

// 				if(height !== 0) {
// 					nodeSet["" + currentEdges[i].id] = true;
// 					filterAncestorsRecurse(currentEdges[i].id, nodeSet, edges, height - 1, costFilter);
// 				}

// 			}

// 			if(("" + currentEdges[i].id) in nodeSet) {
// 				edges.push({
// 					from: currentEdges[i].id,
// 					to: nodeId,
// 					score: currentEdges[i].scoreReadable,
// 					color: currentEdges[i].color,
// 					thickness: currentEdges[i].thickness
// 				});
// 			}
// 		}
// 	}

// 	/**
// 	 * Filter a tree to have only decendants of a particular node.
// 	 * @param  {int} nodeId  Node id of the focal node
// 	 * @param  {int} depth   Depth to limit the tree to. Defaults to unlimited.
// 	 * @param  {float} costFilter   Mimimum cost for node to be included.
// 	 * @return {object}                      A tree object containing 'nodes' and 'edges'.
// 	 */
// 	function filterDescendants(nodeId, depth, costFilter) {
// 		var nodeSet = {}, nodeList = [], edgeList = [];
// 		filterDescendantsRecurse(nodeId, nodeSet, edgeList, depth || -1, costFilter);
// 		for(var i in nodeSet) {
// 			nodeList.push(fulltree.nodes[i-1]);
// 		}
// 		return {nodes: nodeList, edges: edgeList};
// 	}

// 	/**
// 	 * Filter a tree to have only ancestors of a particular node.
// 	 * @param  {int} nodeId  Node id of the focal node
// 	 * @param  {int} height   Height to limit the tree to. Defaults to unlimited.
// 	 * @param  {float} costFilter   Mimimum cost for node to be included.
// 	 * @return {object}                      A tree object containing 'nodes' and 'edges'.
// 	 */
// 	function filterAncestors (nodeId, height, costFilter) {
// 		var nodeSet = {}, nodeList = [], edgeList = [];
// 		filterAncestorsRecurse(nodeId, nodeSet, edgeList, height || -1, costFilter);
// 		for(var i in nodeSet) {
// 			nodeList.push(fulltree.nodes[i-1]);
// 		}
// 		return {nodes: nodeList, edges: edgeList};
// 	}

// 	/**
// 	 * Get edges for the Call-tree
// 	 * @return {array} Array of edges {from, to}
// 	 */
// 	this.getEdges = function() {
// 		return fulltree.edges;
// 	}

// 	/**
// 	 * Get nodes for the call-tree
// 	 * @return {array} Array of nodes {id, label, level, score}
// 	 */
// 	this.getNodes = function() {
// 		return fulltree.nodes;
// 	}

// 	/**
// 	 * Get a node by function name
// 	 * @param  {string} func Function name
// 	 * @return {object}      Node if found, otherwise null
// 	 */
// 	this.getNodeByFunctionName = function(func)
// 	{
// 		var nodes = fulltree.nodes;
// 		for (var i = 0; i < nodes.length; i++) {
// 			if(nodes[i].data.location.function == func) {
// 				return nodes[i];
// 			}
// 		}
// 		return null;
// 	}

// 	/**
// 	 * Get a node by its node id
// 	 * @param  {int} nodeId Node id to search for
// 	 * @return {object}        Node
// 	 */
// 	this.getNodeById = function(nodeId) {
// 		return fulltree.nodes[nodeId-1];
// 	}

// 	/**
// 	 * Filter a tree to have only the ancestors and decendants for a particular node.
// 	 * @param  {int} nodeId Node id of focal node
// 	 * @param  {int} depth  Depth to limit to. Defaults to unlimited.
// 	 * @param  {int} height Height to limit to. Defaults to unlimited.
// 	 * @param  {float} costFilterPercentage Minimum cost in percentage for node to be included.
// 	 * @param  {string} metric               Type of metric to use as score.
// 	 * @param {boolean} isRatio Should the node scores be calculated as percentages?
// 	 * @return {object}                      A tree object containing 'nodes' and 'edges'.
// 	 */
// 	this.filterNodeLine = function(nodeId, depth, height, costFilterPercentage, metric, isRatio) {
// 		addScores(fulltree.nodes, metric, isRatio);
// 		addColorCodes(fulltree);

// 		var max = -1;
// 		for (var i = 0; i < fulltree.nodes.length; i++) {
// 			if(fulltree.nodes[i].score > max) {
// 				max = fulltree.nodes[i].score;
// 			}
// 		}
// 		var min = max + 1;
// 		for (var i = 0; i < fulltree.nodes.length; i++) {
// 			if(fulltree.nodes[i].score < min) {
// 				min = fulltree.nodes[i].score;
// 			}
// 		}

// 		var childrenCostFilter = new CostFilter(costFilterPercentage, [min, fulltree.nodes[nodeId-1].score, max], metric);
// 		var parentCostFilter = new CostFilter(costFilterPercentage, [min, max], metric);
// 		var descs = filterDescendants(nodeId, depth, childrenCostFilter);
// 		var ancs = filterAncestors(nodeId, height, parentCostFilter);

// 		var edgeSet = {};
// 		for (var i = 0; i < descs.edges.length; i++) {
// 			edgeSet[descs.edges[i].from + ',' + descs.edges[i].to] = descs.edges[i];
// 		}
// 		for (var i = 0; i < ancs.edges.length; i++) {
// 			edgeSet[ancs.edges[i].from + ',' + ancs.edges[i].to] = ancs.edges[i];
// 		}

// 		var edgeList = [];
// 		for(var i in edgeSet) {
// 			edgeList.push(edgeSet[i]);
// 		}

// 		return {nodes: union(descs.nodes, ancs.nodes), edges: edgeList};
// 	}

// 	/**
// 	 * Filter a tree to get all root nodes plus their descendants
// 	 * @param  {int}    depth                Depth to limit to. Defaults to unlimited.
// 	 * @param  {int}    costFilterPercentage Minimum cost in percentage for node to be included.
// 	 * @param  {string} metric               Type of metric to use as score.
// 	 * @param {boolean} isRatio Should the node scores be calculated as percentages?
// 	 * @return {object}                      A tree object containing 'nodes' and 'edges'.
// 	 */
// 	this.filterRootLines = function(depth, costFilterPercentage, metric, isRatio) {
// 		addScores(fulltree.nodes, metric, isRatio);
// 		addColorCodes(fulltree);

// 		var max = -1;
// 		for (var i = 0; i < fulltree.nodes.length; i++) {
// 			if(fulltree.nodes[i].score > max) {
// 				max = fulltree.nodes[i].score;
// 			}
// 		}
// 		var min = max + 1;
// 		for (var i = 0; i < fulltree.nodes.length; i++) {
// 			if(fulltree.nodes[i].score < min) {
// 				min = fulltree.nodes[i].score;
// 			}
// 		}

// 		var nodeSet = {}, edgeList = [];
// 		for (var i = 0; i < fulltree.nodes.length; i++) {
// 			if(fulltree.nodes[i].inEdges.length == 0) {
// 				var childrenCostFilter = new CostFilter(costFilterPercentage, [min, fulltree.nodes[i].score, max], metric);
// 				filterDescendantsRecurse(fulltree.nodes[i].id, nodeSet, edgeList, depth, childrenCostFilter);
// 			}
// 		}

// 		var edgeSet = {};
// 		var edges = [];
// 		var nodes = [];
// 		for (var i = 0; i < edgeList.length; i++) {
// 			edgeSet[edgeList[i].from + ',' + edgeList[i].to] = edgeList[i];
// 		}
// 		for(var i in edgeSet) {
// 			edges.push(edgeSet[i]);
// 		}
// 		for(var i in nodeSet) {
// 			nodes.push(fulltree.nodes[i-1]);
// 		}

// 		return {nodes: nodes, edges: edges};
// 	}

// 	// console.time("buildCallTree");
// 	var calltree = buildCallTree(stacktree);
// 	// console.timeEnd("buildCallTree");

// 	// console.time("generateTreeDataSet");
// 	var fulltree = generateTreeDataSet(calltree);
// 	// console.timeEnd("generateTreeDataSet");

// 	// console.time("addColorCodes");
// 	// console.timeEnd("addColorCodes");

// 	return this;
// }

// module.exports = CallTreeAdapter;
