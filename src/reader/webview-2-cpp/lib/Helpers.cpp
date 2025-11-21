/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/webview-2-cpp/lib/BasicAuth.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <string>
#include <vector>
#include <libgen.h>
#include <sys/stat.h>
#include "Helpers.hpp"

/**********************************************************/
namespace MALTWebviewCpp
{

/**********************************************************/
void makedirs(const std::string & path)
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
