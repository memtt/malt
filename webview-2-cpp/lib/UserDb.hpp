/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#ifndef MALT_WEBVIEW_CPP_USER_HANDLER_HPP
#define MALT_WEBVIEW_CPP_USER_HANDLER_HPP

/**********************************************************/
#include <string>
#include <map>
#include <openssl/sha.h>

/**********************************************************/
namespace MALTWebviewCpp
{

/**********************************************************/
class UserDb
{
	public:
		UserDb(void);
		UserDb(const std::string & dbPath, bool warn = true);
		~UserDb(void);
		bool check(const std::string & login, const std::string & password) const;
		void set(const std::string & login, const std::string & password);
		bool has(const std::string & login) const;
		void save(void) const;
	private:
		void makedirs(const std::string & path) const;
		void load(const std::string & path, bool warn);
		static std::string calcDefaultDbPath(void);
		static std::string sha256sum(const std::string & value);
	private:
		std::string dbPath;
		std::map<std::string, std::string> users;
};

}

#endif //MALT_WEBVIEW_CPP_USER_HANDLER_HPP
