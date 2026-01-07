/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/callgraph/DotCode.hpp
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

#ifndef MALT_READER_DOT_CODE_HPP
#define MALT_READER_DOT_CODE_HPP

/**********************************************************/
#include <map>
#include <list>
#include <string>
#include <memory>
#include <functional>

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
class DotCode;

/**********************************************************/
struct DotEntry
{
	std::string type;
	std::string name;
	std::string id;
	std::string from;
	std::string to;
	std::shared_ptr<DotCode> tree;
	std::map<std::string, std::string> properties;

};

/**********************************************************/
/**
 * Generate dot code via a fluent style
 *
 * Dot code is compatible with Graphviz. Currently only
 * a limited language is supported for the use-case of
 * generating a simple tree diagram.
 *
 * For reference, see www.graphviz.org/doc/info/lang.html
 */
class DotCode
{
	public:
		static bool isEmptyObject(const std::map<std::string, std::string> & obj);
		static std::string generatePropList(const std::map<std::string, std::string> & props);
		DotCode & digraph(const std::string & name, std::function<void(DotCode &)> callback);
		DotCode & node(const std::string & id, const std::map<std::string, std::string> & prop);
		DotCode & edge(const std::string & from, const std::string & to, const std::map<std::string, std::string> & prop);
		DotCode & nodeStyle(const std::map<std::string, std::string> & prop);
		DotCode & edgeStyle(const std::map<std::string, std::string> & prop);
		std::string toCode();
	private:
		std::list<DotEntry> listing;
		std::map<std::string, std::string> nodeStyles;
		std::map<std::string, std::string> edgeStyles;
};

}

#endif //MALT_READER_DOT_CODE_HPP
