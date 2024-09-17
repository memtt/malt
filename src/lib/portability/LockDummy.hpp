/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.2
*    DATE     : 06/2023
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/portability/LockDummy.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

#ifndef MALT_MUTEX_DUMMY_HPP
#define MALT_MUTEX_DUMMY_HPP

/**********************************************************/
namespace MALT
{

/**********************************************************/
struct LockDummy
{
	LockDummy(void) {};
	~LockDummy(void) {};
	void lock(void) {};
	void unlock(void) {};
	bool tryLock(void) {return true;};
};

}

#endif //MALT_MUTEX_DUMMY_HPP
