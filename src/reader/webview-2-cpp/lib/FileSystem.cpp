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
#include "config.h"
#ifdef MALT_HAVE_STDCXX17_FILESYTEM
	#include <filesystem>
#else
	#include <dirent.h>
	#include <unistd.h>
	#include <sys/stat.h>
	#include <libgen.h>
#endif
#include "FileSystemPosix.hpp"
#include "FileSystem.hpp"


/**********************************************************/
namespace MALTWebviewCpp
{

/**********************************************************/
bool fsFileRemove(const std::string & path)
{
	#ifdef MALT_HAVE_STDCXX17_FILESYTEM
		return std::filesystem::remove(path + "/index.html");
	#else
		int status = unlink(path.c_str());
		if (status == 0);
	#endif
}

/**********************************************************/
bool fsRemoveAll(const std::string & path)
{
	#ifdef MALT_HAVE_STDCXX17_FILESYTEM
		return std::filesystem::remove_all(path);
	#else
		return fsPosixRemoveAll(path);
	#endif
}

/**********************************************************/
bool fsExists(const std::string& path) {
	#ifdef MALT_HAVE_STDCXX17_FILESYTEM
		return std::filesystem::exists(path);
	#else
		struct stat buffer;
		return (stat(path.c_str(), &buffer) == 0);
	#endif
}

/**********************************************************/
bool fsCreateDirectories(const std::string & path)
{
	#ifdef MALT_HAVE_STDCXX17_FILESYTEM
		return std::filesystem::create_directories(path);
	#else
		return fsCreateDirectoriesPosix(path);
	#endif
}

/**********************************************************/
std::string fsFilename(const std::string & path)
{
	#ifdef MALT_HAVE_STDCXX17_FILESYTEM
		return std::filesystem::path(path).filename();
	#else
		return basename((char*)path.c_str());
	#endif
}

/**********************************************************/
bool fsCopyFile(const std::string & source, const std::string destination)
{
	#ifdef MALT_HAVE_STDCXX17_FILESYTEM
		return std::filesystem::copy_file(source, destination);
	#else
		return fsCopyFilePosix(source, destination);
	#endif
}

}
