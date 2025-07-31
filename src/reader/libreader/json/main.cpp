/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 03/2025
*    LICENSE  : CeCILL-C
*    FILE     : ./src/reader/libreader/json/main.cpp
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
