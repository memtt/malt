/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//internals
#include "CompilerGNU.hpp"
#include <common/Debug.hpp>
//standard
#include <cstdlib>
#include <cstdio>
//compiler
#include <cxxabi.h>

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*******************  FUNCTION  *********************/
std::string CompilerGNU::demangleCppNames(const std::string& value)
{
	//vars
	int status;
	
	//trivial
	if (value.empty())
	{
		MALT_WARNING("Get empty value as name for demangling !");
		return value;
	} else if (value[0] != '_' || value[1] != 'Z') {
		return value;
	}
	
	//call
	char * res = abi::__cxa_demangle(value.c_str(),NULL,NULL,&status);
	
	//errors
	if (status != 0 || res == NULL)
	{
		MALT_WARNING_ARG("Failed to demangle '%1' with compiler function, get error %2 !").arg(value).arg(status).end();
		return value;
	} else if (value == res) {
		return value;
	} else {
		std::string tmp = res;
		free(res);
		return tmp;
	}
}

}
