/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 05/2026
*    LICENSE  : CeCILL-C
*    FILE     : cmake/has_filesystem.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ISTerre & IPAG / UGA / CNRS) - 2026
***********************************************************/

/**********************************************************/
#include <filesystem>

/**********************************************************/
int main()
{
	std::filesystem::path path("/usr/bin/bash");
	return 0;
}
