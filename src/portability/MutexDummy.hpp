/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_MUTEX_DUMMY_HPP
#define MATT_MUTEX_DUMMY_HPP

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  CLASS  **********************/
struct MutexDummy
{
	MutexDummy(void) {};
	~MutexDummy(void) {};
	void lock(void) {};
	void unlock(void) {};
	bool tryLock(void) {return true;};
};

}

#endif //MATT_MUTEX_DUMMY_HPP
