/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/webview-2-cpp/lib/ArgChecker.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
*    AUTHOR   : Sébastien Valat - 2026
***********************************************************/

/**********************************************************/
#include <stdexcept>
#include <iostream>
#include <nlohmann/json.hpp>
#include "ArgChecker.hpp"

/**********************************************************/
using namespace httplib;

/**********************************************************/
namespace MALTWebviewCpp
{

/**********************************************************/
static bool str_replace_at_start(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if(start_pos != 0)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

/**********************************************************/
ArgChecker::ArgChecker(void)
{
}

/**********************************************************/
ArgChecker::~ArgChecker(void)
{

}

/**********************************************************/
std::string ArgChecker::checkArgString(const httplib::Request& req, const std::string & name, bool allowEmpty) const
{
	//do not have
	if (req.has_param(name.c_str()) == false) {
		throw std::runtime_error(std::string("Missing field : ") + name);
	}
	//is empty
	if (req.get_param_value(name.c_str()).empty() && allowEmpty == false) {
		throw std::runtime_error(std::string("Empty field : ") + name);
	}
	//ok
	return req.get_param_value(name.c_str());
}

/**********************************************************/
std::string ArgChecker::checkArgPath(const httplib::Request& req, const std::string & name) const
{
	//get value
	std::string value = this->checkArgString(req, name);

	//check in allowed list
	const auto it = this->pathAllowed.find(value);
	if (it == this->pathAllowed.end()) {
		throw std::runtime_error(std::string("Field ") + name + std::string(" reference a file not in the white list !"));
	}

	//replace path
	for (const auto & it : this->pathOverriding) {
		value = str_replace_at_start(value, it.first, it.second);
	}

	//ok
	return value;
}

/**********************************************************/
std::string ArgChecker::checkJsonArgString(const httplib::Request& req, const std::string & name, bool allowEmpty) const
{
	//get body as json
	const nlohmann::json json = nlohmann::json::parse(req.body);

	//wrong format
	if (json.is_object() == false) {
		throw std::runtime_error(std::string("Invalid body format for : ") + name);
	}

	//do not have
	if (json.contains(name) == false) {
		throw std::runtime_error(std::string("Missing field : ") + name);
	}
	//is empty
	if (json[name].empty() && allowEmpty == false) {
		throw std::runtime_error(std::string("Empty field : ") + name);
	}
	//ok
	return json[name];
}

/**********************************************************/
std::string ArgChecker::checkJsonArgPath(const httplib::Request& req, const std::string & name) const
{
	//get value
	std::string value = this->checkJsonArgString(req, name);

	//check in allowed list
	const auto it = this->pathAllowed.find(value);
	if (it == this->pathAllowed.end()) {
		throw std::runtime_error(std::string("Field ") + name + std::string(" reference a file not in the white list !"));
	}

	//replace path
	for (const auto & it : this->pathOverriding) {
		str_replace_at_start(value, it.first, it.second);
	}

	//ok
	return value;
}

/**********************************************************/
void ArgChecker::overridePath(const std::string origin, const std::string & replacement)
{
	//make copy to modify
	std::string originCopy = origin;
	std::string replacementCopy = replacement;

	//check end slash
	if (originCopy[originCopy.size() - 1] != '/') {
		originCopy += "/";
	}
	if (replacementCopy[replacementCopy.size() - 1] != '/') {
		replacementCopy += "/";
	}

	//insert
	this->pathOverriding[originCopy] = replacementCopy;
}

/**********************************************************/
void ArgChecker::allowPath(const std::string & path)
{
	this->pathAllowed.insert(path);
}

/**********************************************************/
void ArgChecker::allowPaths(const std::map<std::string, bool> & paths)
{
	for (const auto & it : paths) {
		if (it.second)
			this->allowPath(it.first);
	}
}

/**********************************************************/
void ArgChecker::overridePaths(const std::list<std::string> & paths)
{
	for (const auto & it : paths) {
		ssize_t separatorPos = it.find(':');
		if (separatorPos != it.npos) {
			const std::string orig = it.substr(0, separatorPos);
			const std::string overrided = it.substr(separatorPos + 1, it.size() - separatorPos - 1);
			this->overridePath(orig, overrided);
		}
	}
}

}
