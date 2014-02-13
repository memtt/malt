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
#include <ostream>

/*******************  NAMESPACE  ********************/
namespace MATT
{

struct Helpers
{
	static void printValue(std::ostream & out,double value, const char* unit = "");
};

};

#endif //MATT_HELPERS_HPP
