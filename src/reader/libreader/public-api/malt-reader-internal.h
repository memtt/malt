/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 04/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/public-api/malt-reader-internal.h
*-----------------------------------------------------------
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
