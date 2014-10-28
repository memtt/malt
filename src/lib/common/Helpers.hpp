/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_HELPERS_HPP
#define MATT_HELPERS_HPP

/********************  HEADERS  *********************/
//STL C++
#include <ostream>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/********************  STRUCT  **********************/
/**
 * Struct to aggregate all global helper functions (might be better to be a namespace).
**/
struct Helpers
{
	static void printValue(std::ostream & out,double value, const char* unit = "");
	static int getFileId(void);
};

};

#endif //MATT_HELPERS_HPP
