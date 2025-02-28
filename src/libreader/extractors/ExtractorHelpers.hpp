/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_READER_EXTRACTOR_HELPER_HPP
#define MALT_READER_EXTRACTOR_HELPER_HPP

/**********************************************************/
#include "format/MaltProfile.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
struct ExtractorHelpers
{
	static bool isAllocFunction(const std::string & name);
};

}

#endif //MALT_READER_EXTRACTOR_HELPER_HPP
