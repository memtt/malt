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
#include <algorithm> 
#include <cctype>
#include <locale>
#include <cstring>
#include "CppDeclParser.hpp"

/**********************************************************/
#define START_END_TO_START_SIZE(start, end) (start), ((end) - (start))

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
// trim from start (in place)
inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

/**********************************************************/
// trim from end (in place)
inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

/**********************************************************/
// trim from end (in place)
inline void trim(std::string &s) {
	rtrim(s);
	ltrim(s);
}

/**********************************************************/
/**
 * @todo Use C++20 ends width when time will have passed.
 */
bool CppDeclParser::endsWith(const std::string & value, const std::string & endsBy)
{
	//trivial
	if (endsBy.size() > value.size())
		return false;

	//std case compare by end reverse
	return std::equal(endsBy.rbegin(), endsBy.rend(), value.rbegin());
}

/**********************************************************/
/**
 * Get the type parameter start index for a CPP prototype string
 * @param  {string} func Prototype declaration string
 * @return {int}         Start index
 */
ssize_t CppDeclParser::getParameterStartIndex(const std::string & func)
{
	//vars
	ssize_t gtCount = 1;
	ssize_t i = func.size() - 2;

	while(gtCount > 0 && i >= 0) {
		if(func[i] == '>') {
			gtCount++;
		} else if(func[i] == '<') {
			gtCount--;
		}
		i--;
	}

	if(gtCount != 0)
		return -1;

	return i + 1;
}

/**
 * Get description for a C++ prototype declaration
 * @param  {string} func Prototype Declaration string
 * @return {object}      Description of the function
 */
FuncDescription CppDeclParser::parseCppPrototype(std::string func)
{
	//var
	FuncDescription funcDescription;
	
	//trim & init full
	ltrim(func);
	rtrim(func);
	funcDescription.full = func;

	// Check if prototype is actually a missing and this is a memory description
	if(endsWith(func, "]")) {
		funcDescription.funcName = func;
		return funcDescription;
	}

	// Check if the function is constant
	if(endsWith(func, "const")) {

		func = func.substr(0, func.size() - strlen("const."));
		trim(func);
		funcDescription.isConst = true;
	}

	bool doesHaveBraces = (func[func.size()-1] == ')');

	// Get args list
	if(doesHaveBraces) {
		funcDescription.arguments = func.substr(START_END_TO_START_SIZE(func.find_last_of('(')+1, func.size()-1));
		func = func.substr(START_END_TO_START_SIZE(0, func.find_last_of('(')));
		trim(func);
		if(!funcDescription.arguments.size()) {
			funcDescription.arguments = "";
		}
	}

	bool isFunctionParameterized = (func[func.size()-1] == '>');

	// Get function type parameters
	if(isFunctionParameterized) {
		size_t parameterStartIndex = getParameterStartIndex(func);
		funcDescription.funcParameters = func.substr(START_END_TO_START_SIZE(parameterStartIndex+1, func.size()-1));
		func = func.substr(START_END_TO_START_SIZE(0, parameterStartIndex));
	}

	size_t doubleColonPosition = func.find_last_of("::");
	bool hasClass = (doubleColonPosition != -1) && (func.substr(doubleColonPosition).find_last_of(' ') == -1);

	// Get function name if there is class
	if(hasClass) {
		funcDescription.funcName = func.substr(doubleColonPosition + 1);
		trim(funcDescription.funcName);
		func = func.substr(START_END_TO_START_SIZE(0, doubleColonPosition-1));
	}

	bool isClassParameterized = (func[func.size()-1] == '>');

	// Get the class type parameters if there is class
	if(hasClass && isClassParameterized) {
		size_t parameterStartIndex = getParameterStartIndex(func);
		funcDescription.classParameters = func.substr(START_END_TO_START_SIZE(parameterStartIndex+1, func.size()-1));
		func = func.substr(START_END_TO_START_SIZE(0, parameterStartIndex));
	}

	size_t lastSpacePosition = func.find_last_of(' ');
	bool hasReturnValue = (lastSpacePosition != -1);

	// Get class name and namespace
	if(hasClass) {
		std::string className = func;

		if(hasReturnValue) {
			className = func.substr(lastSpacePosition + 1);
			func = func.substr(0, lastSpacePosition);
		}

		// Is namespaced?
		if(className.find_last_of("::") != -1) {
			funcDescription.nameSpace = className.substr(START_END_TO_START_SIZE(0, className.find_last_of("::")-1));
 			className = className.substr(className.find_last_of("::") + 1);
		}

		funcDescription.className = className;
	}

	// Get function name if no class
	if(!hasClass) {
		if(hasReturnValue) {
			funcDescription.funcName = func.substr(lastSpacePosition);
			trim(funcDescription.funcName);
			func = func.substr(0, lastSpacePosition);
		} else {
			funcDescription.funcName = func;
		}
	}

	// Get return type if any
	if(hasReturnValue) {
		funcDescription.returnType = func;
		trim(funcDescription.returnType);
	}

	return funcDescription;
}

/**
 * Get a short name for a method from its description object
 * @param  {object} funcDescription Description object
 * @return {string}                 Short name
 */
std::string CppDeclParser::getShortName(const FuncDescription & funcDescription) {
	std::string name;

	if(funcDescription.nameSpace.empty() == false) {
		name += funcDescription.nameSpace + "::";
	}

	if(funcDescription.className.empty() == false) {
		name += funcDescription.className;
		if(funcDescription.classParameters.empty() == false) {
			name += "<...>";
		}
		name += "::";
	}


	name += funcDescription.funcName;

	if(funcDescription.funcParameters.empty() == false) {
		name += "<...>";
	}

	if(funcDescription.arguments.empty() == false) {
		name += "(...)";
	} else if(!endsWith(funcDescription.funcName, "]")) {
		name += "()";
	}

	if (funcDescription.full.size() > name.size())
		return name;
	else
		return funcDescription.full;
}

}
