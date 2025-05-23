/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/common/Helpers.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
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

	//SKIP FOR NOW
	bool finalStatus = true;
	for (auto & job : jobList)
		if (job.run() == false)
			finalStatus = false;
	return finalStatus;

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
