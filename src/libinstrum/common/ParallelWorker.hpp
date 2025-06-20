/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
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
#include <atomic>
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
	std::atomic<size_t> cursor{0};

	//spawn threads
	std::list<std::thread> workers;
	for (size_t i = 0 ; i < threads ; i++) {
		workers.emplace_back([&jobList, &cursor, &finalStatus](){
			//check has one
			while (1) {
				//take the data
				size_t currentCursor = cursor.fetch_add(1);
			
				//make it if has one or break
				if (currentCursor >= jobList.size()) {
					return;
				} else {
					auto current = jobList[currentCursor];
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
