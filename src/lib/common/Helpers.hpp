/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_HELPERS_HPP
#define MATT_HELPERS_HPP

/********************  HEADERS  *********************/
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

/********************  MACRO  ***********************/
#define foreach(type,list,it) for(type::iterator it = (list).begin() ; it != (list).end ; ++it)
#define foreachConst(type,list,it) for(type::const_iterator it = (list).begin() ; it != (list).end() ; ++it)

};

#endif //MATT_HELPERS_HPP
