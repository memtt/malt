/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.0
*    DATE     : 03/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/state/ReentranceGuard.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#ifndef MALT_REENTRANCE_GUARD_HPP
#define MALT_REENTRANCE_GUARD_HPP

/**********************************************************/
#include "LazyEnv.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
class ReentranceGuard
{
	public:
		inline ReentranceGuard(LazyEnv & env);
		inline ~ReentranceGuard(void);
		inline bool needInstrument(void) const;
		inline bool useInternalAlloc(void) const;
	private:
		bool oldStatus;
		LazyEnv & env;
};

/**********************************************************/
ReentranceGuard::ReentranceGuard(LazyEnv & env)
	:env(env)
{
	this->oldStatus = this->env.markInstrumented();
}

/**********************************************************/
ReentranceGuard::~ReentranceGuard(void)
{
	this->env.endInstrumentation(this->oldStatus);
}

/**********************************************************/
bool ReentranceGuard::needInstrument(void) const
{
	return this->oldStatus == false && this->env.isMaltReady();
}

/**********************************************************/
bool ReentranceGuard::useInternalAlloc(void) const
{
	return false;
	return this->oldStatus == true;// && this->env.isMaltReady();
}

}

#endif //MALT_REENTRANCE_GUARD_HPP

