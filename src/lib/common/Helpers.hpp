/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.5
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/common/Helpers.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

#ifndef MALT_HELPERS_HPP
#define MALT_HELPERS_HPP

/**********************************************************/
//STL C++
#include <ostream>

/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * Struct to aggregate all global helper functions (might be better to be a namespace).
 * 
 * @brief Simple container to aggregate some helper functions.
**/
struct Helpers
{
	static void printValue(std::ostream & out,double value, const char* unit = "");
	static int getFileId(void);
};

}

#endif //MALT_HELPERS_HPP
