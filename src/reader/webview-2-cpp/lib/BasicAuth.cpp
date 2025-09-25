/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/webview-2-cpp/lib/BasicAuth.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <base64.h>
#include "BasicAuth.hpp"

/**********************************************************/
using namespace httplib;

/**********************************************************/
namespace MALTWebviewCpp
{

/**********************************************************/
BasicAuth::BasicAuth(const std::string & realm)
	:realm(realm)
{
}

/**********************************************************/
BasicAuth::BasicAuth(const std::string & realm, const std::string & dbPath, bool warn)
	:realm(realm)
	,userDb(dbPath, warn)
{

}

/**********************************************************/
BasicAuth::~BasicAuth(void)
{

}

/**********************************************************/
bool BasicAuth::check(const std::string & authenticate) const
{
	try {
		auto auth = this->decode(authenticate);
		return this->userDb.check(auth.first, auth.second);
	} catch (std::runtime_error & error) {
		return false;
	}
}

/**********************************************************/
Server::HandlerResponse BasicAuth::check(const httplib::Request& req, httplib::Response& res) const
{
	//statis
	bool status = false;

	//check
	if (req.has_header("Authorization") == false) {
		status = false;
	} else {
		status = this->check(req.get_header_value("Authorization"));
	}

	//status
	if (status) {
		return Server::HandlerResponse::Unhandled;
	} else {
		res.set_header("www-authenticate", std::string("Basic realm=\"") + this->realm + std::string("\""));
		res.set_content("error", "text/html");
		res.status = StatusCode::Unauthorized_401;
		return Server::HandlerResponse::Handled;
	}
}

/**********************************************************/
std::pair<std::string, std::string> BasicAuth::decode(const std::string & authenticate)
{
	//trivial
	if (authenticate.empty())
		throw new std::runtime_error("Empty authenticate !");

	//need stat with 'Basic '
	if (authenticate.substr(0, 6) != "Basic ")
		throw new std::runtime_error("Non supported authentication value !");

	//extract
	const std::string authValue = authenticate.substr(6, authenticate.size() - 6);
	const std::string authDecoded = base64_decode(authValue);

	//search sep
	auto sep = authDecoded.find(':');
	if (sep == std::string::npos)
		throw std::runtime_error("Fail to find ':' to separate login:pass !");

	//split
	const std::string login = authDecoded.substr(0, sep);
	const std::string pass = authDecoded.substr(sep + 1, authDecoded.size() - sep - 1);

	//check
	return std::pair<std::string, std::string>(login, pass);
}

}
