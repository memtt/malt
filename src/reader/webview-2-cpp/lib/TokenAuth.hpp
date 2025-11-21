/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/webview-2-cpp/lib/BasicAuth.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#ifndef MALT_WEBVIEW_CPP_TOKEN_AUTH_HPP
#define MALT_WEBVIEW_CPP_TOKEN_AUTH_HPP

/**********************************************************/
#include <httplib.h>

/**********************************************************/
namespace MALTWebviewCpp
{

/**********************************************************/
class TokenAuth
{
	public:
		TokenAuth(bool regenToken = false);
		~TokenAuth(void);
		bool check(const std::string & authenticate) const;
		httplib::Server::HandlerResponse check(const httplib::Request& req, httplib::Response& res) const;
		std::string getToken(void) const;
	private:
		static std::string loadToken(bool regrenToken = false);
		static std::string regenToken(void);
		static std::string decode(const std::string & authenticate);
		static std::string getTokenFilePath(void);
		static std::string generateRandomKey(void);
	private:
		std::string expectedToken;
};

}

#endif //MALT_WEBVIEW_CPP_TOKEN_AUTH_HPP
