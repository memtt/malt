/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/json/main.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "File.hpp"

/**********************************************************/
int main(int argc, char * argv[])
{
	MALTJson::JsonFile file(argv[1]);
	return EXIT_SUCCESS;
}
