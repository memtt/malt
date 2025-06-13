/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.0
*    DATE     : 11/2014
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/portability/LockDummy.hpp
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
