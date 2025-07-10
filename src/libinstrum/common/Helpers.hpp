/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/Helpers.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014 - 2018
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
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
	static std::string loadFullFile(const std::string & fname);
	static bool writeFullFile(const std::string & fname, const std::string & data);
	static size_t valueFromKiloMegaGiga(const std::string & value);
};

}

#endif //MALT_HELPERS_HPP
