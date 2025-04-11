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

#ifndef MALT_READER_CALL_TREE_ADAPTER_HPP
#define MALT_READER_CALL_TREE_ADAPTER_HPP

/**********************************************************/
#include "FuncMetrics.hpp"
#include "extractors/Extractor.hpp"
#include "SimpleIdCache.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
struct InOutEdge
{
	InOutEdge(size_t id, const MALTFormat::StackInfos & stats):id(id), stats(stats){};
	ssize_t from{0};
	ssize_t to{0};
	size_t id{0};
	MALTFormat::StackInfos stats;
	std::string color;
	double thickness{0.0};
	double score{0.0};
	std::string scoreReadable;
};

/**********************************************************/
struct CallTreeEdge
{
	CallTreeEdge(size_t from, size_t to): from(from), to(to) {};
	size_t from{0};
	size_t to{0};
};

/**********************************************************/
struct CallTreeNode
{
	CallTreeNode(size_t id, std::string label, std::string tooltip, size_t level, MALTFormat::StackInfos stats, const FullTreeNode & treeNode);
	CallTreeNode(const CallTreeNode & orig) = default;
	size_t id{0};
	std::string label;
	std::string tooltip;
	size_t level{0};
	MALTFormat::StackInfos stats;
	const FullTreeNode & treeNode;
	std::vector<InOutEdge> outEdges;
	std::vector<InOutEdge> inEdges;
	double score{0.0};
	std::string scoreReadable;
	std::string color;
};

/**********************************************************/
struct Graph
{
	std::vector<CallTreeNode> nodeList;
	std::vector<InOutEdge> edgeList;
};

/**********************************************************/
struct TreeSet
{
	std::vector<CallTreeNode> nodes;
	std::vector<CallTreeEdge> vertices;
};

/**********************************************************/
class CostFilter
{
	public:
		CostFilter(float cost, float min, float max, const MaltMetric & metric);
		CostFilter(float cost, float min, float middle, float max, const MaltMetric & metric);
		bool check(float value) const;
	private:
		void setup(float cost, float min, float max, const MaltMetric & metric);
	private:
		bool greaterThan;
		float costValue;
};

/**********************************************************/
/**
 * An adapter class that encapsulates a stack-tree and exposes
 * it as nodes and edges, suitable for making a calltree graph.
 *
 * @param {object} stacktree Stack tree object
 */
class CallTreeAdapter
{
	public:
		CallTreeAdapter(const Extractor & extractor);
		~CallTreeAdapter(void);
		size_t generateNodesAndVertices(const FullTreeNode & treeNode, size_t level, std::vector<CallTreeNode> & nodes, std::vector<CallTreeEdge> & vertices, SimpleIdCache & nodeCache, SimpleIdCache & vertCache, std::vector<size_t> stack);
		TreeSet generateTreeDataSet(FullTreeNode & treeNode);
		static std::string convertRgbStringToHex(const std::string & rgb);
		static void addColorCodes(TreeSet & dataset);
		void addScores(std::vector<CallTreeNode> & nodes, const MaltMetric & metric, bool isRatio);
		void filterDescendantsRecurse(ssize_t nodeId, std::map<ssize_t, bool> nodeSet, std::vector<InOutEdge> & edges, size_t depth, const CostFilter & costFilter);
		void filterAncestorsRecurse(ssize_t nodeId, std::map<ssize_t, bool> nodeSet, std::vector<InOutEdge> & edges, size_t height, const CostFilter & costFilter);
		Graph filterDescendants(ssize_t nodeId, ssize_t depth, const CostFilter & costFilter);
		Graph filterAncestors (ssize_t nodeId, ssize_t height, const CostFilter & costFilter);
		const std::vector<CallTreeEdge> & getEdges(void) const;
		const std::vector<CallTreeNode> & getNodes(void) const;
		const CallTreeNode * getNodeByFunctionName(const std::string & func);
		const CallTreeNode * getNodeById(ssize_t nodeId);
		Graph filterNodeLine(ssize_t nodeId, ssize_t depth, ssize_t height, double costFilterPercentage, const MaltMetric & metric, bool isRatio);
		Graph filterRootLines(ssize_t depth, double costFilterPercentage, const MaltMetric & metric, bool isRatio);
	private:
		const Extractor & extractor;
		FullTreeNode calltree;
		TreeSet fulltree;
};

}

#endif //MALT_READER_CALL_TREE_ADAPTER_HPP
