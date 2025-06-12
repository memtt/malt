/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/ParallelWorker.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_PARALLEL_WORKER_HPP
#define MALT_PARALLEL_WORKER_HPP

/**********************************************************/
//STL C++
#include <mutex>
#include <thread>

/**********************************************************/
namespace MALT
{

/**********************************************************/
template <class T>
bool runParallelJobs(T & jobList, int threads)
{
	//check
	assert(threads > 0);
	assert(threads < 2048);

	//vars
	bool finalStatus = true;

	//SKIP FOR NOW
	/*for (auto & job : jobList)
		if (job.run() == false)
			finalStatus = false;
	return finalStatus;*/

	//iterator
	std::mutex mutex;
	typename T::iterator it = jobList.begin();

	//spawn threads
	std::list<std::thread> workers;
	for (size_t i = 0 ; i < threads ; i++) {
		workers.emplace_back([&jobList, &mutex, &it, &finalStatus](){
			//check has one
			while (1) {
				//take the data
				mutex.lock();
				typename T::iterator current = it++;
				mutex.unlock();
			
				//make it if has one or break
				if (current == jobList.end()) {
					return;
				} else {
					bool status = current->run();
					if (status == false)
						finalStatus = false;
				}
			}
		});
	}

	//wait all
	for (auto & worker : workers)
		worker.join();

	//ok
	return finalStatus;
}

}

#endif //MALT_PARALLEL_WORKER_HPP
