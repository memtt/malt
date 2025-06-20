/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 11/2014
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/portability/CompilerGNU.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

#ifndef MALT_COMPILER_GNU_HPP
#define MALT_COMPILER_GNU_HPP

/**********************************************************/
//standard
#include <string>

/**********************************************************/
namespace MALT
{

/**********************************************************/
class CompilerGNU
{
	public:
		static std::string demangleCppNames(const std::string & value);
};

}

#endif //MALT_COMPILER_GNU_HPP
