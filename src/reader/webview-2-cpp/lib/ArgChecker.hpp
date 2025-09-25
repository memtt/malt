/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 09/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/webview-2-cpp/lib/ArgChecker.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#ifndef MALT_WEBVIEW_CPP_ARG_CHECKER_HPP
#define MALT_WEBVIEW_CPP_ARG_CHECKER_HPP

/**********************************************************/
#include <map>
#include <string>
#include <httplib.h>

/**********************************************************/
namespace MALTWebviewCpp
{

/**********************************************************/
class ArgChecker
{
	public:
		ArgChecker(void);
		~ArgChecker(void);
		void overridePath(const std::string origin, const std::string & replacement);
		void overridePaths(const std::list<std::string> & paths);
		void allowPath(const std::string & path);
		void allowPaths(const std::map<std::string, bool> & paths);
		std::string checkArgString(const httplib::Request& req, const std::string & name, bool allowEmpty = false) const;
		std::string checkArgPath(const httplib::Request& req, const std::string & name) const;
		std::string checkJsonArgString(const httplib::Request& req, const std::string & name, bool allowEmpty = false) const;
		std::string checkJsonArgPath(const httplib::Request& req, const std::string & name) const;
	private:
		std::map<std::string, std::string> pathOverriding;
		std::set<std::string> pathAllowed;
};

}

#endif //MALT_WEBVIEW_CPP_BASIC_AUTH_HPP
