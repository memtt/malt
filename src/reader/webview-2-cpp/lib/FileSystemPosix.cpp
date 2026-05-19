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
//std c
#include <cstring>
//std C++
#include <fstream>
//unix posix
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <libgen.h>
//malt
#include "FileSystemPosix.hpp"

/**********************************************************/
namespace MALTWebviewCpp
{

/**********************************************************/
bool fsPosixRemoveAll(const std::string& path) {
	//open directory
	DIR* dir = opendir(path.c_str());
	if (!dir) {
		//not a dir, try to delete
		return (unlink(path.c_str()) == 0);
	}

	//is directory, loop on entries to remove them recursively
	struct dirent* entry;
	while ((entry = readdir(dir)) != nullptr) {
		//skip current and parent dir
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
			continue;
		}

		//build path
		std::string full_path = path + "/" + entry->d_name;
		struct stat stat_buf;
		if (stat(full_path.c_str(), &stat_buf) == 0) {
			if (S_ISDIR(stat_buf.st_mode)) {
				if (fsPosixRemoveAll(full_path) == false)
					break;
			} else {
				if (unlink(full_path.c_str()) != 0 )
					break;
			}
		}
	}

	closedir(dir);
	return (rmdir(path.c_str()) == 0);
}

/**********************************************************/
bool fsCreateDirectoriesPosix(const std::string & path)
{
	size_t pos = 0;
	std::string current_path;
	std::string token;

	while ((pos = path.find('/', pos)) != std::string::npos) {
		token = path.substr(0, pos);
		if (!token.empty()) {
			struct stat st;
			if (stat(token.c_str(), &st) != 0) {
				if (mkdir(token.c_str(), 0755) != 0) {
					return false;
				}
			} else if (!S_ISDIR(st.st_mode)) {
				return false;
			}
		}
		pos++;
	}

	// Créer le dernier répertoire si nécessaire
	struct stat st;
	if (stat(path.c_str(), &st) != 0) {
		return (mkdir(path.c_str(), 0755) == 0);
	}
	return S_ISDIR(st.st_mode);
}

/**********************************************************/
bool fsCopyFilePosix(const std::string & source, const std::string destination)
{
	//remove the dest file if present
	unlink(destination.c_str());

	//open source
	std::ifstream src_file(source.c_str(), std::ios::binary);
	if (!src_file) {
		return false;
	}

	//open dest
	std::ofstream dest_file(destination.c_str(), std::ios::binary | std::ios::trunc);
	if (!dest_file) {
		return false;
	}

	//copy content
	dest_file << src_file.rdbuf();

	//check status
	if (!src_file || !dest_file) {
		return false;
	}

	//ok
	return true;
}

}
