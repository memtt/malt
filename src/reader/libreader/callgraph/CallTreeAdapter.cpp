/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/callgraph/CallTreeAdapter.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/********************** AUTHOR NOTE ************************
This file is originally written in pure JS by 
 - Mehdi Raza Jaffery (CERN) - 2016
It has been rewritten in C++ by
 - Sébastien Valat (INRIA) - 2025
/**********************************************************/

/**********************************************************/
#include <algorithm>
#include "extractors/ExtractorHelpers.hpp"
#include "CppDeclParser.hpp"
#include "CallTreeAdapter.hpp"

namespace MALTReader
{

/**********************************************************/
CallTreeNode::CallTreeNode(size_t id, std::string label, std::string tooltip, size_t level, MALTFormat::StackInfos stats, const InstructionInfosStrRef * location)
	:id(id)
	,label(label)
	,tooltip(tooltip)
	,level(level)
	,stats(stats)
	,location(location)
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
	this->calltree = extractor.getFullTree();

	//build graph
	this->fulltree = this->generateTreeDataSet(this->calltree);
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
		if (ExtractorHelpers::isAllocFunction(this->extractor.getProfile().run.allocatorWrappers, *treeNode.location->function))
			return CACHE_ID_NULL;

		//get identifier
		identifier = *treeNode.location->function;

		// Add node to node-list if this is a new node
		if(!nodeCache.exists(identifier)) {
			currentId = nodeCache.put(identifier);
			const std::string label = CppDeclParser::getShortName(CppDeclParser::parseCppPrototype(*treeNode.location->function));
			const std::string tooltip = *treeNode.location->function;
			if (treeNode.infos == nullptr) {
				nodes.emplace_back(currentId, label, tooltip, level, MALTFormat::StackInfos(), treeNode.location);
			} else {
				nodes.emplace_back(currentId, label, tooltip, level, *treeNode.infos, treeNode.location);
			}
		} else {
			currentId = nodeCache.get(identifier);
			if(getIndexOf(stack, stack.size(), currentId) == -1) {
				assert(currentId - 1 < nodes.size());
				if (treeNode.infos != nullptr)
					nodes[currentId - 1].stats.merge(*treeNode.infos);
			}
		}
	}

	// Add current node to stack
	if(currentId != CACHE_ID_NULL)
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
				MALTFormat::StackInfos infos;
				if (it.second.infos != nullptr)
					infos.merge(*it.second.infos);
				nodes[currentId-1].outEdges.emplace_back(childId, infos);
				nodes[childId-1].inEdges.emplace_back(currentId, infos);

				//add vertice
				vertices.emplace_back(currentId, childId);
			} else if(childId != CACHE_ID_NULL && vertCache.exists(vertName) && getIndexOf(stack, level-1, currentId) == -1 && getIndexOf(stack, level-1, childId) == -1) {
				// Find existing edge and merge stats
				for (ssize_t j = nodes[currentId-1].outEdges.size() - 1; j >= 0; j--) {
					if(nodes[currentId-1].outEdges[j].id == childId) {
						if (it.second.infos != nullptr)
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
						if (it.second.infos != nullptr)
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

/**********************************************************/
/**
 * Convert RGB string to HEX color string
 * @param  {string} rgb RGB color string
 * @return {string}     HEX color string
 */
std::string CallTreeAdapter::convertRgbStringToHex(const std::string & rgb) {
	int r = 0, g = 0, b = 0;
	if (sscanf(rgb.c_str(), "(%d,%d,%d)", &r, &g, &b) == 3) {
		char buffer[256];
		snprintf(buffer, sizeof(buffer), "#%02x%02x%02x", r, g, b);
		return buffer;
	} else {
		return "#000000";
	}
}

/**********************************************************/
D3ScaleLinearRange::D3ScaleLinearRange(double valueMin, double valueMax, double rangeMin, double rangeMax)
{
	this->valueMax = valueMax;
	this->valueMin = valueMin;
	this->rangeMax = rangeMax;
	this->rangeMin = rangeMin;
}

/**********************************************************/
double D3ScaleLinearRange::getValue(double value) const
{
	//calc delta
	double delta = this->rangeMax - this->rangeMin; 
	double ratio = (value - rangeMin) / delta;

	//calc colors
	return this->valueMin + (double)(this->valueMax - this->valueMin) * ratio;
}

/**********************************************************/
D3ScaleLinearColorRange::D3ScaleLinearColorRange(const std::string & colorMin, const std::string & colorMax, double rangeMin, double rangeMax)
{
	//parse
	ssize_t res1 = sscanf(colorMin.c_str(), "#%02x%02x%02x", &colorMin_r, &colorMin_g, &colorMin_b);
	ssize_t res2 = sscanf(colorMax.c_str(), "#%02x%02x%02x", &colorMax_r, &colorMax_g, &colorMax_b);

	//check
	assert(res1 == 3);
	assert(res2 == 3);

	this->rangeMin = rangeMin;
	this->rangeMax = rangeMax;
}

/**********************************************************/
std::string D3ScaleLinearColorRange::getColor(double value) const
{
	//calc delta
	double delta = this->rangeMax - this->rangeMin; 
	double ratio = (value - rangeMin) / delta;

	//calc colors
	int r = this->colorMin_r + (double)(this->colorMax_r - this->colorMin_r) * ratio;
	int g = this->colorMin_g + (double)(this->colorMax_g - this->colorMin_g) * ratio;
	int b = this->colorMin_b + (double)(this->colorMax_b - this->colorMin_b) * ratio;

	//make web color
	char buffer[256];
	snprintf(buffer, sizeof(buffer), "#%02x%02x%02x", r, g, b);

	//ok
	return buffer;
}

/**********************************************************/
/**
 * Calculates color codes for nodes and edges. based on a metric
 *
 * Color is added as the 'color' property on each node and edge.
 * 'Thickness' is also added to the edges.
 *
 * @param {dataset} dataset Call-Tree dataset
 */
void CallTreeAdapter::addColorCodes(TreeSet & dataset)
{
	//vars
	std::vector<CallTreeNode> & nodes = dataset.nodes;
	ssize_t max = -1;

	// find max
	for (const auto & node : nodes)
		max = std::max((ssize_t)node.score, max);

	// generate a mapping function from [0-max] onto [#397EBA,#FF9595]
	D3ScaleLinearColorRange colorScale = D3ScaleLinearColorRange("#397EBA", "#ab4141", 0, max);

	// generate a mapping function from [0-max] onto [1,4]
	D3ScaleLinearRange thicknessScale = D3ScaleLinearRange(0.8, 8, 0, max);

	// assign colors
	for (auto & node : nodes) {
		node.color = convertRgbStringToHex(colorScale.getColor(node.score));
		for (auto & inEdge : node.inEdges) {
			inEdge.color = convertRgbStringToHex(colorScale.getColor(inEdge.score));
			inEdge.thickness = thicknessScale.getValue(inEdge.score);
		}
		for (auto & outEdge : node.outEdges) {
			outEdge.color = convertRgbStringToHex(colorScale.getColor(outEdge.score));
			outEdge.thickness = thicknessScale.getValue(outEdge.score);
		}
	}
}

/**********************************************************/
/**
 * Add score attribute to nodes
 * @param {array} nodes  Node list
 * @param {string} metric Type of metric to use as score
 * @param {boolean} isRatio Should the score be calculated as percentages?
 */
void CallTreeAdapter::addScores(std::vector<CallTreeNode> & nodes, const MaltMetric & metric, bool isRatio)
{
	//vars
	auto extractValue = metric.extractor;
	auto formatValue = metric.formater;

	//apply
	if(isRatio) {
		double max = 0;
		for (const auto & node : fulltree.nodes) {
			if(node.inEdges.size() == 0) {
				size_t val = metric.extractor(node.stats);
				if(metric.ref == REF_MODE_SUM) {
					max += val;
				} else {
					max = val > max? val : max;
				}
			}
		}
		for (auto & node : nodes) {
			node.score = extractValue(node.stats)/max*100.0;
			char buffer[256];
			snprintf(buffer, sizeof(buffer), "%.02g%%", (double)(node.score*100.0)/100.0);
			node.scoreReadable = buffer;

			for (size_t j = 0; j < node.inEdges.size(); j++) {
				node.inEdges[j].score = extractValue(node.inEdges[j].stats)/max*100.0;
				snprintf(buffer, sizeof(buffer), "%.02g%%", (double)(node.inEdges[j].score*100.0)/100.0);
				node.inEdges[j].scoreReadable = buffer;
			}
			for (size_t j = 0; j < node.outEdges.size(); j++) {
				node.outEdges[j].score = extractValue(node.outEdges[j].stats)/max*100.0;
				snprintf(buffer, sizeof(buffer), "%.02g%%", (double)(node.outEdges[j].score*100.0)/100.0);
				node.outEdges[j].scoreReadable = buffer;
			}
		}
	} else {
		for (auto & node : nodes) {
			node.score = extractValue(node.stats);
			node.scoreReadable = formatValue(node.score);

			for (size_t j = 0; j < node.inEdges.size(); j++) {
				node.inEdges[j].score = extractValue(node.inEdges[j].stats);
				node.inEdges[j].scoreReadable = formatValue(node.inEdges[j].score);
			}
			for (size_t j = 0; j < node.outEdges.size(); j++) {
				node.outEdges[j].score = extractValue(node.outEdges[j].stats);
				node.outEdges[j].scoreReadable = formatValue(node.outEdges[j].score);
			}
		}
	}
}

/**********************************************************/
/**
 * Filter a tree to have only decendants of a particular node.
 * @param  {int} nodeId  Node id of the focal node
 * @param  {object} nodeSet Set of nodes already in graph
 * @param  {array} edges   Set of edges already in graph
 * @param  {int} depth   Depth to limit the tree to. Defaults to unlimited.
 * @param  {float} costFilter   Mimimum cost for node to be included.
 */
void CallTreeAdapter::filterDescendantsRecurse(ssize_t nodeId, std::map<ssize_t, bool> nodeSet, std::vector<InOutEdge> & edges, size_t depth, const CostFilter & costFilter)
{
	//init
	nodeSet[nodeId] = true;

	std::vector<InOutEdge> & currentEdges = fulltree.nodes[nodeId-1].outEdges;
	fprintf(stderr, "CURRENT_EDGED = %zu\n", currentEdges.size());
	for (size_t i = 0; i < currentEdges.size(); i++) {
		if(nodeSet.find(currentEdges[i].id) != nodeSet.end()) {
			if(depth != 0) {
				if(!costFilter.check(fulltree.nodes[currentEdges[i].id-1].score))
					continue;

				nodeSet[currentEdges[i].id] = true;

				filterDescendantsRecurse(currentEdges[i].id, nodeSet, edges, depth - 1, costFilter);
			}
		}

		if(nodeSet.find(currentEdges[i].id) != nodeSet.end()) {
			InOutEdge edge = currentEdges[i];
			edge.from = nodeId;
			edge.to = edge.id;
			edges.emplace_back(edge);
		}
	}
	fprintf(stderr, "EDGED = %zu\n", edges.size());
}

/**********************************************************/
/**
 * Filter a tree to have only ancestors of a particular node.
 * @param  {int} nodeId  Node id of the focal node
 * @param  {object} nodeSet Set of nodes already in graph
 * @param  {array} edges   Set of edges already in graph
 * @param  {int} height   height to limit the tree to. Defaults to unlimited.
 * @param  {float} costFilter   Mimimum cost for node to be included.
 */
void CallTreeAdapter::filterAncestorsRecurse(ssize_t nodeId, std::map<ssize_t, bool> nodeSet, std::vector<InOutEdge> & edges, size_t height, const CostFilter & costFilter)
{
	nodeSet[nodeId] = true;

	std::vector<InOutEdge> & currentEdges = fulltree.nodes[nodeId-1].inEdges;
	for (size_t i = 0; i < currentEdges.size(); i++) {
		if(nodeSet.find(currentEdges[i].id) != nodeSet.end()) {
			if(!costFilter.check(fulltree.nodes[currentEdges[i].id-1].score))
				continue;

			if(height != 0) {
				nodeSet[currentEdges[i].id] = true;
				filterAncestorsRecurse(currentEdges[i].id, nodeSet, edges, height - 1, costFilter);
			}

		}

		if(nodeSet.find(currentEdges[i].id) != nodeSet.end()) {
			InOutEdge edge = currentEdges[i];
			edge.from = edge.id;
			edge.to = nodeId;
			edges.emplace_back(edge);
		}
	}

	fprintf(stderr, "EDGED2 = %zu\n", edges.size());
}

/**********************************************************/
/**
 * Filter a tree to have only decendants of a particular node.
 * @param  {int} nodeId  Node id of the focal node
 * @param  {int} depth   Depth to limit the tree to. Defaults to unlimited.
 * @param  {float} costFilter   Mimimum cost for node to be included.
 * @return {object}                      A tree object containing 'nodes' and 'edges'.
 */
Graph CallTreeAdapter::filterDescendants(ssize_t nodeId, ssize_t depth, const CostFilter & costFilter)
{
	Graph graph;
	std::map<ssize_t, bool> nodeSet;
	filterDescendantsRecurse(nodeId, nodeSet, graph.edgeList, depth || -1, costFilter);
	for(const auto & it : nodeSet) {
		graph.nodeList.emplace_back(fulltree.nodes[it.first-1]);
	}
	fprintf(stderr, "GRAPH FILTER DESC = %zu\n", graph.nodeList.size());
	return graph;
}

/**********************************************************/
/**
 * Filter a tree to have only ancestors of a particular node.
 * @param  {int} nodeId  Node id of the focal node
 * @param  {int} height   Height to limit the tree to. Defaults to unlimited.
 * @param  {float} costFilter   Mimimum cost for node to be included.
 * @return {object}                      A tree object containing 'nodes' and 'edges'.
 */
Graph CallTreeAdapter::filterAncestors (ssize_t nodeId, ssize_t height, const CostFilter & costFilter)
{
	Graph graph;
	std::map<ssize_t, bool> nodeSet;
	filterAncestorsRecurse(nodeId, nodeSet, graph.edgeList, height, costFilter);
	for(const auto & it : nodeSet) {
		graph.nodeList.emplace_back(fulltree.nodes[it.first-1]);
	}
	fprintf(stderr, "GRAPH FILTER ANCEST = %zu\n", graph.nodeList.size());
	return graph;
}

/**********************************************************/
/**
 * Get edges for the Call-tree
 * @return {array} Array of edges {from, to}
 */
const std::vector<CallTreeEdge> & CallTreeAdapter::getEdges(void) const
{
	return fulltree.vertices;
}

/**********************************************************/
/**
 * Get nodes for the call-tree
 * @return {array} Array of nodes {id, label, level, score}
 */
const std::vector<CallTreeNode> & CallTreeAdapter::getNodes(void) const
{
	return fulltree.nodes;
}

/**********************************************************/
/**
 * Get a node by function name
 * @param  {string} func Function name
 * @return {object}      Node if found, otherwise null
 */
const CallTreeNode * CallTreeAdapter::getNodeByFunctionName(const std::string & func)
{
	const std::vector<CallTreeNode> & nodes = fulltree.nodes;
	for (auto & node : nodes) {
		//if(node.data.location.function == func) {
		if(node.tooltip == func) {
			return &node;
		}
	}
	return nullptr;
}

/**********************************************************/
/**
 * Get a node by its node id
 * @param  {int} nodeId Node id to search for
 * @return {object}        Node
 */
const CallTreeNode * CallTreeAdapter::getNodeById(ssize_t nodeId)
{
	if (nodeId < 0 || nodeId >= fulltree.nodes.size())
		return nullptr;
	else
		return &fulltree.nodes[nodeId-1];
}

/**********************************************************/
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
Graph CallTreeAdapter::filterNodeLine(ssize_t nodeId, ssize_t depth, ssize_t height, double costFilterPercentage, const MaltMetric & metric, bool isRatio) {
	addScores(fulltree.nodes, metric, isRatio);
	addColorCodes(fulltree);

	double max = -1;
	for (const auto & node : fulltree.nodes) {
		if(node.score > max) {
			max = node.score;
		}
	}
	double min = max + 1;
	for (const auto & node : fulltree.nodes) {
		if(node.score < min) {
			min = node.score;
		}
	}

	CostFilter childrenCostFilter(costFilterPercentage, min, fulltree.nodes[nodeId-1].score, max, metric);
	CostFilter parentCostFilter(costFilterPercentage, min, max, metric);
	Graph descs = filterDescendants(nodeId, depth, childrenCostFilter);
	Graph ancs = filterAncestors(nodeId, height, parentCostFilter);
	Graph finalGraph;

	std::map<std::string, InOutEdge> edgeSet = {};
	char buffer[512];
	for (size_t i = 0; i < descs.edgeList.size(); i++) {
		snprintf(buffer, sizeof(buffer), "%zd-%zd", descs.edgeList[i].from, descs.edgeList[i].to);
		edgeSet.emplace(buffer, descs.edgeList[i]);
	}
	for (size_t i = 0; i < ancs.edgeList.size(); i++) {
		snprintf(buffer, sizeof(buffer), "%zd-%zd", ancs.edgeList[i].from, ancs.edgeList[i].to);
		edgeSet.emplace(buffer, ancs.edgeList[i]);
	}
	for(const auto & it : edgeSet) {
		finalGraph.edgeList.emplace_back(it.second);
	}

	std::map<ssize_t, CallTreeNode> nodeSet = {};
	for (size_t i = 0; i < descs.nodeList.size(); i++) {
		nodeSet.emplace(descs.nodeList[i].id, descs.nodeList[i]);
	}
	for (size_t i = 0; i < ancs.edgeList.size(); i++) {
		nodeSet.emplace(ancs.nodeList[i].id, ancs.nodeList[i]);
	}
	for(const auto & it : nodeSet) {
		finalGraph.nodeList.emplace_back(it.second);
	}

	return finalGraph;
}

/**********************************************************/
/**
 * Filter a tree to get all root nodes plus their descendants
 * @param  {int}    depth                Depth to limit to. Defaults to unlimited.
 * @param  {int}    costFilterPercentage Minimum cost in percentage for node to be included.
 * @param  {string} metric               Type of metric to use as score.
 * @param {boolean} isRatio Should the node scores be calculated as percentages?
 * @return {object}                      A tree object containing 'nodes' and 'edges'.
 */
Graph CallTreeAdapter::filterRootLines(ssize_t depth, double costFilterPercentage, const MaltMetric & metric, bool isRatio)
{
	addScores(fulltree.nodes, metric, isRatio);
	addColorCodes(fulltree);

	double max = -1;
	for (const auto & node : fulltree.nodes) {
		if(node.score > max) {
			max = node.score;
		}
	}
	double min = max + 1;
	for (const auto & node : fulltree.nodes) {
		if(node.score < min) {
			min = node.score;
		}
	}

	std::vector<InOutEdge> edgeList;
	std::map<ssize_t, bool> nodeSet;
	for (const auto & node : fulltree.nodes) {
		if(node.inEdges.size() == 0) {
			CostFilter childrenCostFilter(costFilterPercentage, min, node.score, max, metric);
			filterDescendantsRecurse(node.id, nodeSet, edgeList, depth, childrenCostFilter);
		}
	}

	std::map<std::string, InOutEdge> edgeSet;
	Graph graph;
	for (auto & edge : edgeList) {
		char buffer[256];
		snprintf(buffer, sizeof(buffer), "%zd-%zd", edge.from, edge.to);
		edgeSet.emplace(buffer,edge);
	}
	for(const auto & it : edgeSet) {
		graph.edgeList.emplace_back(it.second);
	}
	for(const auto & it : nodeSet) {
		graph.nodeList.emplace_back(fulltree.nodes.at(it.first-1));
	}

	return graph;
}

}

