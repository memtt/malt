/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/public-api/malt-reader.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
*    AUTHOR   : Sébastien Valat - 2026
***********************************************************/

/**********************************************************/
#include "../api/WebProfile.hpp"
#include "malt-reader.h"

/**********************************************************/
using namespace MALTReader;

/**********************************************************/
struct malt_reader_t
{
	WebProfile * profile{nullptr};
	bool verbose{false};
};
