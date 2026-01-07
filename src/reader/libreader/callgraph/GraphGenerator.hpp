/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/callgraph/GraphGenerator.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
*    AUTHOR   : Sébastien Valat - 2026
***********************************************************/

/********************** AUTHOR NOTE ************************
This file is originally written in pure JS by 
 - Mehdi Raza Jaffery (CERN) - 2016
It has been rewritten in C++ by
 - Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_READER_GRAPH_GENERATOR_HPP
#define MALT_READER_GRAPH_GENERATOR_HPP

/**********************************************************/
#include <string>
#include "CallTreeAdapter.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
class GraphGenerator
{
	public:
		static void replaceAllSubStr(std::string & in, const std::string & what, const std::string & byWhat);
		static std::string safeTagsReplace(const std::string & str);
		static std::string getDotCodeForTree(const Graph & tree, ssize_t focusedNode);
		static std::string convertDotToSvg(const std::string & dotCode);
		static bool isDotInstalled();
};

}

#endif // MALT_READER_GRAPH_GENERATOR_HPP
