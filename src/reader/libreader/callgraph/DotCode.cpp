/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/callgraph/DotCode.cpp
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
#include <sstream>
#include "DotCode.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
bool DotCode::isEmptyObject(const std::map<std::string, std::string> & obj)
{
	return obj.empty();
}

/**********************************************************/
std::string DotCode::generatePropList(const std::map<std::string, std::string> & props)
{
	//empty case
	if (props.empty())
		return "";

	//vars
	std::stringstream buffer;
	bool needComa = false;

	//fill
	buffer << " [";
	for(const auto & it : props) {
		if (needComa)
			buffer << ", ";
		else
			needComa = true;
		buffer << it.first << "=\"" << it.second << '"';
	}
	buffer << "]";

	//ok
	return buffer.str();
}

/**********************************************************/
DotCode & DotCode::digraph(const std::string & name, std::function<void(DotCode &)> callback)
{
	DotEntry entry;
	entry.type = "digraph";
	entry.name = name;
	entry.tree = std::shared_ptr<DotCode>(new DotCode());
	callback(*entry.tree);
	listing.emplace_back(entry);
	return *this;
}

/**********************************************************/
DotCode & DotCode::node(const std::string & id, const std::map<std::string, std::string> & prop)
{
	DotEntry entry;
	entry.type = "node";
	entry.id = id;
	entry.properties = prop;
	listing.emplace_back(entry);
	return *this;
}

/**********************************************************/
DotCode & DotCode::edge(const std::string & from, const std::string & to, const std::map<std::string, std::string> & prop)
{
	DotEntry entry;
	entry.type = "edge";
	entry.from = from;
	entry.to = to;
	entry.properties = prop;
	listing.emplace_back(entry);
	return *this;
}

/**********************************************************/
DotCode & DotCode::nodeStyle(const std::map<std::string, std::string> & prop)
{
	for (const auto & it : prop)
		this->nodeStyles[it.first] = it.second;
	return *this;
}

/**********************************************************/
DotCode & DotCode::edgeStyle(const std::map<std::string, std::string> & prop)
{
	for (const auto & it : prop)
		this->edgeStyles[it.first] = it.second;
	return *this;
}

/**********************************************************/
std::string DotCode::toCode()
{
	std::stringstream buffer;

	if(!isEmptyObject(nodeStyles)) {
		buffer << "node " << generatePropList(nodeStyles) << ";" << std::endl;
	}

	if(!isEmptyObject(edgeStyles)) {
		buffer << "edge " << generatePropList(edgeStyles) << ";" << std::endl;
	}

	for (const auto & x : listing) {
		if(x.type == "digraph") {
			buffer << "digraph " << x.name << " {" << std::endl;
			buffer << x.tree->toCode() << std::endl;
			buffer << "}" << std::endl;
		} else if(x.type == "node") {
			buffer << x.id << generatePropList(x.properties) << ";" << std::endl;
		} else if(x.type == "edge") {
			buffer << x.from << " -> " << x.to << generatePropList(x.properties) << ";" << std::endl;
		}
	}

	return buffer.str();
}

}
