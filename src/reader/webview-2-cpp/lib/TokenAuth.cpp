/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/webview-2-cpp/lib/BasicAuth.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <cstdio>
#include <libgen.h>
#include <unistd.h>
#include <sstream>
#include "Helpers.hpp"
#include "TokenAuth.hpp"

/**********************************************************/
using namespace httplib;

/**********************************************************/
namespace MALTWebviewCpp
{

/**********************************************************/
TokenAuth::TokenAuth(bool regenToken)
{
	this->expectedToken = this->loadToken(regenToken);
}

/**********************************************************/
TokenAuth::~TokenAuth(void)
{
}

/**********************************************************/
bool TokenAuth::check(const std::string & authenticate) const
{
	try {
		auto auth = this->decode(authenticate);
		return auth == this->expectedToken;
	} catch (std::runtime_error & error) {
		return false;
	}
}

/**********************************************************/
Server::HandlerResponse TokenAuth::check(const httplib::Request& req, httplib::Response& res) const
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
		res.set_header("www-authenticate", std::string("Bearer"));
		res.set_content("error", "text/html");
		res.status = StatusCode::Unauthorized_401;
		return Server::HandlerResponse::Handled;
	}
}

/**********************************************************/
std::string TokenAuth::decode(const std::string & authenticate)
{
	//trivial
	if (authenticate.empty())
		throw new std::runtime_error("Empty authenticate !");

	//need stat with 'Basic '
	if (authenticate.substr(0, 7) != "Bearer ")
		throw new std::runtime_error("Non supported authentication value !");

	//extract
	const std::string token = authenticate.substr(7, authenticate.size() - 7);

	//check
	return token;
}

/**********************************************************/
std::string TokenAuth::getTokenFilePath(void)
{
	//getenv for home
	const std::string homedir = getenv("HOME");

	//build
	const std::string path = homedir + "/.local/share/malt/token";

	//ok
	return path;
}

/**********************************************************/
std::string TokenAuth::loadToken(bool regenToken)
{
	//regen
	if (regenToken) {
		printf("Regen token...\n");
		return TokenAuth::regenToken();
	}

	//get path
	const std::string path = TokenAuth::getTokenFilePath();

	//load
	FILE * fp = fopen(path.c_str(), "r");

	//not there
	if (fp == nullptr) {
		printf("Genegerate token...\n");
		return TokenAuth::regenToken();
	}

	//load
	char buffer[4096];
	char * status = fgets(buffer, sizeof(buffer), fp);

	//close
	fclose(fp);

	//error
	if (status != buffer)
		throw new std::runtime_error(std::string("Fail to load the token from ") + path + " !");

	//ok
	return buffer;
}

/**********************************************************/
std::string TokenAuth::regenToken(void)
{
	//get path
	const std::string path = TokenAuth::getTokenFilePath();

	//create dirs
	std::string tmp = path;
	makedirs(dirname((char*)tmp.c_str()));

	//gen
	const std::string token = TokenAuth::generateRandomKey();

	//write it
	FILE * fp = fopen(path.c_str(), "w+");
	if (fp == nullptr)
		throw new std::runtime_error(std::string("Fail to open ") + path + " to write the token !");

	//set access limited
	const int status = chmod(path.c_str(), S_IRUSR | S_IWUSR);
	if (status != 0)
		throw new std::runtime_error(std::string("Fail to chmod ") + path + " to secure the token !");

	//write
	const ssize_t size = fwrite(token.c_str(), 1, token.size(), fp);
	if (size != token.size())
		throw new std::runtime_error(std::string("Fail to fully write ") + path + " to store the token !");

	//close
	fclose(fp);

	//ok
	return token;
}

/**********************************************************/
std::string TokenAuth::generateRandomKey(void)
{
	//open
	FILE * fp = fopen("/dev/random", "r");
	if (fp == nullptr)
		throw new std::runtime_error("Fail to open /dev/random !");

	//read N chars
	char buffer[4096];
	const size_t size = 24;
	memset(buffer, 0, sizeof(buffer));

	//read
	const ssize_t status = fread(buffer, size, 1, fp);
	if (status != 1)
		throw new std::runtime_error("Fail to read key from /dev/random !");

	//close
	fclose(fp);

	//to hex
	std::stringstream tokenStream;
	const char hexChars[] = "0123456789abcdef";
	for (size_t i = 0 ; i < size ; i++) {
		const size_t left = ((unsigned char*)buffer)[i] % 16;
		const size_t right = (((unsigned char*)buffer)[i] / 16) % 16;
		tokenStream << hexChars[left];
		tokenStream << hexChars[right];
	}

	//ok
	return tokenStream.str();
}

/**********************************************************/
std::string TokenAuth::getToken(void) const
{
	return this->expectedToken;
}

}
