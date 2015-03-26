/*****************************************************
             PROJECT  : MALT
             VERSION  : 0.1.0
             DATE     : 03/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_BACKTRACE_HPP
#define MALT_BACKTRACE_HPP

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*********************  CLASS  **********************/
class Backtrace
{
	public:
		static int backtrace(void** buffer,int maxDepth);
};

}

#endif //MALT_BACKTRACE_HPP
