/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/webview-2-cpp/lib/UserDb.hpp
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
		UserDb(bool warn = true, bool auto_passwd = true);
		UserDb(const std::string & dbPath, bool warn = true, bool auto_passwd = true);
		~UserDb(void);
		bool check(const std::string & login, const std::string & password) const;
		void set(const std::string & login, const std::string & password);
		bool has(const std::string & login) const;
		void save(void) const;
	private:
		void makedirs(const std::string & path) const;
		void load(const std::string & path, bool warn, bool auto_passwd = true);
		static std::string calcDefaultDbPath(void);
		static std::string sha256sum(const std::string & value);
		static std::string escapeUserNameForShell(const std::string & value);
		static std::string promptUserName(void);
	private:
		std::string dbPath;
		std::map<std::string, std::string> users;
};

}

#endif //MALT_WEBVIEW_CPP_USER_HANDLER_HPP
