/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 09/2025
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
#include "core/ThreadTracker.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * @brief Provide a simple way to run some parallel tasks.
 * 
 * The goal of this light implementation based on std::thread is to run some
 * tasks in parallel when generating the profile. It permits avoiding to depend
 * on OpenMP inside MALT not to conflict with the possible one used in the
 * application we instrument.
 * 
 * @param jobList Define a list of jobs consisting in tasks with a run()
 * function to execute.
 * @param threads The number of thread to use.
 */
template <class T>
bool runParallelJobs(T & jobList, int threads)
{
	//check
	assert(threads > 0);
	assert(threads < 2048);

	//stop tracking threads
	bool oldTrackingValue = MALT::gblThreadTrackerData.trackingIsEnabled;
	MALT::gblThreadTrackerData.trackingIsEnabled = false;

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
	for (int i = 0 ; i < threads ; i++) {
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

	//stop tracking threads
	MALT::gblThreadTrackerData.trackingIsEnabled = oldTrackingValue;

	//ok
	return finalStatus;
}

}

#endif //MALT_PARALLEL_WORKER_HPP
