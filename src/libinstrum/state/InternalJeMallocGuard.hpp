/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : ./src/lib/instrum/wrapper/GlobalState.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#ifndef MALT_JE_MALLOC_GUARD_HPP
#define MALT_JE_MALLOC_GUARD_HPP

/**********************************************************/
#include "LazyEnv.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
class InternalJeMallocGuard
{
	public:
		inline InternalJeMallocGuard(LazyEnv & env);
		inline ~InternalJeMallocGuard(void);
	private:
		bool oldStatus;
		LazyEnv & env;
};

/**********************************************************/
InternalJeMallocGuard::InternalJeMallocGuard(LazyEnv & env)
	:env(env)
{
	this->oldStatus = this->env.markInInternalJeMalloc();
}

/**********************************************************/
InternalJeMallocGuard::~InternalJeMallocGuard(void)
{
	this->env.restoreInInternalJeMalloc(this->oldStatus);
}

}

#endif //MALT_JE_MALLOC_GUARD_HPP
