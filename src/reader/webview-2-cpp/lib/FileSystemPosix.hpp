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
#ifndef MALT_WEBVIEW_CPP_FILE_SYSTEM_POSIX_HPP
#define MALT_WEBVIEW_CPP_FILE_SYSTEM_POSIX_HPP

/**********************************************************/
#include <string>

/**********************************************************/
namespace MALTWebviewCpp
{

/**********************************************************/
bool fsPosixRemoveAll(const std::string& path);
bool fsCreateDirectoriesPosix(const std::string & path);
bool fsCopyFilePosix(const std::string & source, const std::string destination);

}

#endif //MALT_WEBVIEW_CPP_FILE_SYSTEM_POSIX_HPP
