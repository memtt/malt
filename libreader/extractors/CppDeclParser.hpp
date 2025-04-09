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

#ifndef MALT_READER_CPP_DECL_PARSER_HPP
#define MALT_READER_CPP_DECL_PARSER_HPP

/**********************************************************/
#include <string>

/**********************************************************/
namespace MALTReader
{

struct FuncDescription
{
	std::string full;
	std::string arguments;
	std::string className;
	std::string funcName;
	std::string funcParameters;
	std::string classParameters;
	std::string returnType;
	std::string nameSpace;
	bool isConst{false};
};

/**
 * A C++ Method Declaration parser
 * 
 * Breaks a declaration into its components, such as
 * method name, class, namespace, arguments, type parameters
 * and return type. Supports a limited subset of declarations
 * only.
 */
struct CppDeclParser
{
	static bool endsWith(const std::string & value, const std::string & endsBy);
	static ssize_t getParameterStartIndex(const std::string & func);
	static FuncDescription parseCppPrototype(std::string func);
	static std::string getShortName(const FuncDescription & funcDescription);
};

}

#endif //MALT_READER_CPP_DECL_PARSER_HPP
