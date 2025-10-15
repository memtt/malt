/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/webview-2-cpp/lib/UserDb.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <libgen.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <nlohmann/json.hpp>
#include "UserDb.hpp"

/**********************************************************/
using namespace nlohmann;

/**********************************************************/
namespace MALTWebviewCpp
{

/**********************************************************/
UserDb::UserDb(bool warn, bool auto_passwd)
{
	const std::string defaultPath = this->calcDefaultDbPath();
	this->dbPath = defaultPath;
	this->load(defaultPath, warn, auto_passwd);
}

/**********************************************************/
UserDb::UserDb(const std::string & dbPath, bool warn, bool auto_passwd)
{
	this->dbPath = dbPath;
	this->load(dbPath, warn, auto_passwd);
}

/**********************************************************/
UserDb::~UserDb(void)
{
}

/**********************************************************/
bool UserDb::check(const std::string & login, const std::string & password) const
{
	auto it = this->users.find(login);
	if (it == this->users.end()) {
		return false;
	} else {
		const std::string passHashed = this->sha256sum(password);
		return it->second == passHashed;
	}
}

/**********************************************************/
void UserDb::set(const std::string & login, const std::string & password)
{
	this->users[login] = this->sha256sum(password);
}

/**********************************************************/
bool UserDb::has(const std::string & login) const
{
	auto it = this->users.find(login);
	return (it != this->users.end());
}

/**********************************************************/
std::string UserDb::escapeUserNameForShell(const std::string & value)
{
	//escape & replace
	std::string usernameSafe;
	for (const auto it : value) {
		if (it == '"')
			usernameSafe += "\\\"";
		else if (it == '\n')
			{}
		else
			usernameSafe += it;
	}
	return usernameSafe;
}

/**********************************************************/
std::string UserDb::promptUserName(void)
{
	//get user name
	char username[4096] = "";

	//prompt
	while (username[0] == '\0' || username[0] == '\n') {
		//prompt
		printf("Username: ");
		fflush(stdout);

		//get
		char * fgetsStatus = fgets(username, sizeof(username), stdin);
		if (fgetsStatus !=  username)
			throw std::runtime_error("Fail to ask username for malt-passwd !");
	}

	return username;
}

/**********************************************************/
void UserDb::load(const std::string & path, bool warn, bool auto_passwd)
{
	//load
	std::ifstream fileStream(path);

	//fail
	if (fileStream.fail()) {
		if (auto_passwd) {
			//prompt
			printf("\nFirst use : create your password to access the web interface :\n");
			std::string username = this->promptUserName();

			//build command
			std::stringstream cmd;
			cmd << MALT_PREFIX_BIN << "/malt-passwd \"" << escapeUserNameForShell(username) <<  + "\"";

			//call malt-passwd to create the DB
			const int status = system(cmd.str().c_str());
			if (status != 0)
				throw std::runtime_error("Fail to call malt-passwd !");
			this->load(path, warn);
		}
		return;
	}

	//parse
	json data = json::parse(fileStream);

	//check type
	if (data.is_object() == false)
		throw new std::runtime_error("Invalid file content, not a JSON object !");

	// special iterator member functions for objects
	for (auto  it = data.begin() ; it != data.end() ; ++it) {
		this->users[it.key()] = it.value();
	}
}

/**********************************************************/
std::string UserDb::calcDefaultDbPath(void)
{
	//getenv for home
	const std::string homedir = getenv("HOME");

	//build
	const std::string path = homedir + "/.local/share/malt/passwd.json";

	//ok
	return path;
}

/**********************************************************/
std::string UserDb::sha256sum(const std::string & value)
{
	//buffer
	unsigned char hash[SHA256_DIGEST_LENGTH];
    
	//hash
	SHA256((const unsigned char *)value.c_str(), value.size(), hash);

	//convert to text
    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];

	//ok
    return ss.str();
}

/**********************************************************/
void UserDb::save(void) const
{
	//create dirs
	std::string tmp = this->dbPath;
	this->makedirs(dirname((char*)tmp.c_str()));

	//open
	std::ofstream out(this->dbPath);
	chmod(this->dbPath.c_str(), S_IRUSR | S_IWUSR);

	//check
	if (out.fail())
		throw std::runtime_error(std::string("Fail to open user DB file to write : ") + this->dbPath);

	//convert to json
	json data = json::object();
	for (auto & it : this->users) {
		data[it.first] = it.second;
	}

	//write
	out << data;

	//close
	out.close();
}

/**********************************************************/
void UserDb::makedirs(const std::string & path) const
{
	//build list
	std::vector<std::string> dirs;

	//fill
	//char * tmp = realpath(path.c_str(), nullptr);
	std::string real = path;

	//loop
	while(true) {
		//split
		const std::string dir = basename((char*)real.c_str());
		if (dir == "/" || dir == ".") {
			break;
		}

		//handle
		dirs.push_back(real);
		real = dirname((char*)real.c_str());
	}

	//make dirs
	for (auto it = dirs.rbegin() ; it != dirs.rend() ; ++it) {
		mkdir(it->c_str(), S_IRWXU | (S_IRGRP | S_IXGRP) | (S_IROTH | S_IXOTH));
	}
}

}
