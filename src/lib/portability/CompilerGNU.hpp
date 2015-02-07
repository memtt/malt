/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_COMPILER_GNU_HPP
#define MATT_COMPILER_GNU_HPP

/********************  HEADERS  *********************/
//standard
#include <string>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  CLASS  **********************/
class CompilerGNU
{
	public:
		static std::string demangleCppNames(const std::string & value);
};

}

#endif //MATT_COMPILER_GNU_HPP
