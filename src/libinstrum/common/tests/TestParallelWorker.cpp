/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/tests/TestParallelWorker.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include <list>
#include "../ParallelWorker.hpp"

/**********************************************************/
using namespace MALT;

/**********************************************************/
class JobTest
{
	public:
		JobTest(bool ret = true){this->ret = ret;};
		bool run(void) {this->status = true; return this->ret;};
	public:
		bool status{false};
		bool ret{true};
};

/**********************************************************/
TEST(TestParallelWorker, runParallelJobs_seq)
{
	std::list<JobTest> jobs;
	for (size_t i = 0 ; i < 2000 ; i++)
		jobs.emplace_back();
	bool status = MALT::runParallelJobs(jobs, 1);
	ASSERT_TRUE(status);
	for (auto & job : jobs)
		ASSERT_TRUE(job.status);
}

/**********************************************************/
TEST(TestParallelWorker, runParallelJobs_parallel)
{
	std::list<JobTest> jobs;
	for (size_t i = 0 ; i < 2000 ; i++)
		jobs.emplace_back();
	bool status = MALT::runParallelJobs(jobs, 8);
	ASSERT_TRUE(status);
	for (auto & job : jobs)
		ASSERT_TRUE(job.status);
}

/**********************************************************/
TEST(TestParallelWorker, runParallelJobs_error)
{
	std::list<JobTest> jobs;
	for (size_t i = 0 ; i < 1000 ; i++)
		jobs.emplace_back();
	jobs.emplace_back(false);
	for (size_t i = 0 ; i < 1000 ; i++)
		jobs.emplace_back();
	bool status = MALT::runParallelJobs(jobs, 8);
	ASSERT_FALSE(status);
	for (auto & job : jobs)
		ASSERT_TRUE(job.status);
}
