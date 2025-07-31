/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : ./src/reader/webview-2-cpp/lib/UserDb.cpp
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
UserDb::UserDb(void)
{
	const std::string defaultPath = this->calcDefaultDbPath();
	this->dbPath = defaultPath;
	this->load(defaultPath, true);
}

/**********************************************************/
UserDb::UserDb(const std::string & dbPath, bool warn)
{
	this->dbPath = dbPath;
	this->load(dbPath, warn);
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
void UserDb::load(const std::string & path, bool warn)
{
	//load
	std::ifstream fileStream(path);

	//fail
	if (fileStream.fail()) {
		system("malt-passwd-new-cpp");
		this->load(path, warn);
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
	unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, value.c_str(), value.size());
    SHA256_Final(hash, &sha256);
    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
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
