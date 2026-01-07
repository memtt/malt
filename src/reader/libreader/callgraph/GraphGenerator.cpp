/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 12/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/callgraph/GraphGenerator.cpp
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
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <sstream>
#include "DotCode.hpp"
#include "CppDeclParser.hpp"
#include "GraphGenerator.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
static std::string trimString(std::string value)
{
	trim(value);
	return value;
}

/**********************************************************/
template<class T>
static std::string toString(T & value)
{
	std::stringstream buffer;
	buffer << value;
	return buffer.str();
}

/**********************************************************/
void GraphGenerator::replaceAllSubStr(std::string & in, const std::string & what, const std::string & byWhat)
{
	size_t n = 0;
	while ((n = in.find(what, n)) != std::string::npos) {
		in.replace(n, what.size(), byWhat);
		n += byWhat.size();
	}
}

/**********************************************************/
/**
 * Replace unsafe tags with HTML escape chars
 * @param  {string} str Input string
 * @return {string}     Escaped string Output
 */
std::string GraphGenerator::safeTagsReplace(const std::string & str)
{
	std::string result = str;
	replaceAllSubStr(result, "&", "&amp");
	replaceAllSubStr(result, "<", "&lt;");
	replaceAllSubStr(result, ">", "&gt;");
	return result;
}

/**********************************************************/
/**
 * Generate dot code for a tree
 * @param  {object} tree        An object containing an nodes and edges arrays
 * @param  {int} focusedNode    Node id of the node to focus on
 * @return {string}             Dot code for this tree
 */
std::string GraphGenerator::getDotCodeForTree(const Graph & tree, ssize_t focusedNode)
{
	//vars
	const auto & nodes = tree.nodeList;
	const auto & vertices = tree.edgeList;

	DotCode dot;

	return dot
		.digraph("G", [&nodes, &vertices, focusedNode](DotCode & d) {
			// Common styling for all nodes and edges
			d.nodeStyle({
				{"shape", "record"},
				{"fontname", "Courier New"},
				{"style", "filled"},
				{"fontcolor", "white"},
				{"penwidth", "1"},
				{"color", "white"},
				{"fontsize", "10"}
			}).edgeStyle({
				{"fontname", "Courier New"},
				{"fontsize", "9"},
			});

			for (size_t i = 0; i < nodes.size(); i++) {
				std::stringstream label;
				label << trimString(safeTagsReplace(nodes[i].label)) <<  " | " << nodes[i].scoreReadable;

				std::map<std::string, std::string> nodeDat = {
					{"label", label.str()}, 
					{"tooltip", trimString(safeTagsReplace(nodes[i].tooltip))},
					{"fillcolor", nodes[i].color}
				};

				// If focused node, highlight it
				if(nodes[i].id == focusedNode) {
					nodeDat["penwidth"] = "3.5";
					nodeDat["color"] = "#2b2b2b";
				}

				// Add node to dot code
				std::stringstream nodeName;
				nodeName << "node" << nodes[i].id;
				d.node(nodeName.str(), nodeDat);
			}

			// Add all edges to dot code
			for (size_t i = 0; i < vertices.size(); i++) {
				// Add node to dot code
				std::stringstream from, to, label;
				from << "node" << vertices[i].from;
				to << "node" << vertices[i].to;
				label << " " << vertices[i].scoreReadable;
				std::string style = "solid";
				if (vertices[i].hasSkipedNodes)
					style = "dashed";
				d.edge(from.str(), to.str(), {
					{"label", label.str()},
					{"color", toString(vertices[i].color)},
					{"penwidth", toString(vertices[i].thickness)},
					{"fontcolor", toString(vertices[i].color)},
					{"style", style}
				});
			}
		})
		.toCode();
}

/**********************************************************/
/**
 * Convert dot code to SVG drawing
 * @param  {string}   dotCode  Dot code for the graph
 * @param  {Function} callback Call back function which is called with (data,err)
 */
std::string GraphGenerator::convertDotToSvg(const std::string & dotCode)
{
	//create temp file
	char fname[] = "/tmp/tmpMaltDotFileXXXXXX";
	const int status = mkstemp(fname);
	assert(status >= 0);
	if (status < 0)
		return "";
	
	//dump dot in
	FILE * fpFile = fopen(fname, "w+");
	const ssize_t status2 = fwrite(dotCode.c_str(), 1, dotCode.size(), fpFile);
	assert(status2 == dotCode.size());
	if (status2 != dotCode.size())
		return "";
	fclose(fpFile);

	//build command
	std::stringstream dotCommand;
	dotCommand << "dot -Tsvg " << fname;

	//run dot
	FILE * fpDot = popen(dotCommand.str().c_str(), "r");
	assert(fpDot != nullptr);
	if (fpDot == nullptr)
		return "";

	//read
	std::stringstream rbuffer;
	while (!feof(fpDot)) {
		char buffer[4096];
		const ssize_t size = fread(buffer, 1, sizeof(buffer) - 1, fpDot);
		buffer[size] = '\0';
		rbuffer << buffer;
	}
	fclose(fpDot);

	//remove
	unlink(fname);

	//ok
	return rbuffer.str();
}

/**********************************************************/
bool GraphGenerator::isDotInstalled()
{
	const int status = system("dot -V 2>/dev/null >/dev/null");
	return status == 0;
}

}
