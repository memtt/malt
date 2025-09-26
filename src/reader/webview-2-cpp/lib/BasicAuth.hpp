/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/webview-2-cpp/lib/BasicAuth.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#ifndef MALT_WEBVIEW_CPP_BASIC_AUTH_HPP
#define MALT_WEBVIEW_CPP_BASIC_AUTH_HPP

/**********************************************************/
#include <httplib.h>
#include "UserDb.hpp"

/**********************************************************/
namespace MALTWebviewCpp
{

/**********************************************************/
class BasicAuth
{
	public:
		BasicAuth(const std::string & realm);
		BasicAuth(const std::string & realm, const std::string & dbPath, bool warn = true);
		~BasicAuth(void);
		bool check(const std::string & authenticate) const;
		httplib::Server::HandlerResponse check(const httplib::Request& req, httplib::Response& res) const;
	private:
		static std::pair<std::string, std::string> decode(const std::string & authenticate);
	private:
		std::string realm;
		UserDb userDb;
};

}

#endif //MALT_WEBVIEW_CPP_BASIC_AUTH_HPP
