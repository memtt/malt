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
#include "Extractor.hpp"
#include "SimpleIdCache.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
struct InOutEdge
{
	InOutEdge(size_t id, const MALTFormat::StackInfos & stats):id(id), stats(stats){};
	size_t id;
	MALTFormat::StackInfos stats;
};

/**********************************************************/
struct CallTreeEdge
{
	CallTreeEdge(size_t from, size_t to): from(from), to(to) {};
	size_t from;
	size_t to;
};

/**********************************************************/
struct CallTreeNode
{
	CallTreeNode(size_t id, std::string label, std::string tooltip, size_t level, MALTFormat::StackInfos stats, const FullTreeNode & treeNode);
	size_t id;
	std::string label;
	std::string tooltip;
	size_t level;
	MALTFormat::StackInfos stats;
	const FullTreeNode & treeNode;
	std::vector<InOutEdge> outEdges;
	std::vector<InOutEdge> inEdges;
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
		size_t generateNodesAndVertices(const FullTreeNode & treeNode, size_t level, std::vector<CallTreeNode> & nodes, std::vector<CallTreeEdge> vertices, SimpleIdCache & nodeCache, SimpleIdCache & vertCache, std::vector<size_t> stack);
	private:
		const Extractor & extractor;
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

}

#endif //MALT_READER_CALL_TREE_ADAPTER_HPP
