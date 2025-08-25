/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 04/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/InternalLeakTracker.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#ifndef MALT_INTERNAL_LEAK_TRACKER_HPP
#define MALT_INTERNAL_LEAK_TRACKER_HPP

/**********************************************************/
#include "common/STLInternalAllocator.hpp"
#include "stacks/BacktraceStack.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * @brief Helper to debug MALT internal memory leaks.
 * 
 * This class permit to track the memory used in MALT and report the leaks at exit.
 * It was usefull for the python suppport adding due to reference counting issues encountered
 * during the dev.
 * 
 * In MALT it is enabled at compile time using the ENABLE_CODE_LEAK option on CMake / Configure.
 */
class InternalLeakTracker
{
	public:
		/**
		 * Constructor of the internal leak tracking.
		 * @param track Enable of disable the tracking by default.
		*/
		InternalLeakTracker(bool track = false) {
			this->track = track;
		}
		/**
		 * Destructor of the internal memory tracker.
		 */
		~InternalLeakTracker(void) {};
		/**
		 * When using malloc, register the pointer to the leak tracker so we know it is in use.
		 * It will be listed as leak if not de-registered by onFree() or onRealloc() when freed.
		 * @param ptr Address of the block to track.
		 * @param size Size of the block to track.
		 */
		void onMalloc(void * ptr, size_t size)
		{
			//ignore bocks when not tracking
			if (!enabled)
				return;

			//incr
			gblLocalCnt++;
			gblLocalMem+=size;

			//print some
			if (gblLocalCnt % 1000 == 0)
				fprintf(stderr, "MALT: internal stat : %zu : %g MB (%zu)\n", gblLocalCnt.load(), (double)gblLocalMem.load()/1024.0/1024.0, size);

			//register
			gblBlockSizes[ptr] = size;

			//register stack of allocation
			stack.loadCurrentStack();
			gblBlockStack.emplace(ptr, stack);

			//check if need to dump
			this->dump();
		}
		/**
		 * De-register the given block to mark it as freed.
		 * @param ptr The address of the block to mark as freed.
		 */
		void onFree(void * ptr)
		{
			//ignore bocks when not tracking
			if (!enabled)
				return;
			
			//count
			size_t size = gblBlockSizes[ptr];
			gblLocalCnt--;
			gblLocalMem-=size;

			//de-register
			gblBlockSizes.erase(ptr);
			gblBlockStack.erase(ptr);

			//print sometime
			if (gblLocalCnt % 1000 == 0)
				fprintf(stderr, "MALT: internal stat : %zu : %g MB (%zu)\n", gblLocalCnt.load(), (double)gblLocalMem.load()/1024.0/1024.0, size);

			//dump
			this->dump();
		}
		/**
		 * Not that we called realloc() on the given memory block.
		 * @param ptr The address of the block to change.
		 * @param size The new size passed to realloc so we can adapt the block size in the registry.
		 */
		void onRealloc(void * ptr, size_t size)
		{
			//ignore bocks when not tracking
			if (!enabled)
				return;

			//update counters
			gblLocalCnt+=0;
			gblLocalMem-=gblBlockSizes[ptr];

			//deregister
			gblBlockSizes.erase(ptr);

			//is free()
			if (size == 0)
				gblLocalCnt--;

			//is malloc()
			if (ptr == nullptr)
				gblLocalCnt++;

			//print sometime
			if (gblLocalCnt % 1000 == 0)
				fprintf(stderr, "MALT: internal stat : %zu : %g MB (%zu)\n", gblLocalCnt.load(), (double)gblLocalMem.load()/1024.0/1024.0, size);

			//update size
			gblBlockSizes[ptr] = size;

			//register allocation stack
			stack.loadCurrentStack();
			gblBlockStack.emplace(ptr, stack);

			//dump
			this->dump();
		}
		/**
		 * Dump when reaching a limit.
		 */
		void dump(void)
		{
			//if reach a limit in amount or count, dump
			if (this->gblLocalMem > 2000UL*1024UL*1204UL || this->gblLocalCnt > 10000) {
				//solve the symbols
				SymbolSolver solver;
				solver.loadProcMap();
				for (const auto & it : this->gblBlockStack)
					it.second.solveSymbols(solver);
				solver.solveNames();


				//debug
				/*for (const auto & it : this->gblBlockStack)
					it.second.printSymbols(solver);*/

				//reduce
				std::set<String, std::less<String>, MALT::STLInternalAllocator<String> > reducer;
				for (const auto & it : this->gblBlockStack)
					reducer.insert(it.second.toDebugString(solver));

				//print
				for (const auto & it : reducer)
					fprintf(stderr, "%s", it.c_str());

				//stop
				abort();
			}
		}
	private:
		/** Is enabled or not */
		const bool enabled = false;
		/** Do we need to track the allocated block or ignore them. */
		bool track{false};
		/** Keep track of the amount of memory allocated by MALT. */
		std::atomic<size_t> gblLocalMem{0};
		/** Keep track of the number of memory blocks allocated by MALT. */
		std::atomic<size_t> gblLocalCnt{0};
		/** Keep track of the size of blocks by identifying the blocks by their addreess */
		std::map<void*, size_t, std::less<void*>, MALT::STLInternalAllocator<std::pair<void*, size_t> > > gblBlockSizes;
		/** Keep the stack making the allocation of the blocks by identifying the blocks by their addreess */
		std::map<void*, Stack, std::less<void*>, MALT::STLInternalAllocator<std::pair<void*, Stack> > > gblBlockStack;
		/** Object ot make stack bactracing */
		BacktraceStack stack;
};

}

#endif //MALT_INTERNAL_LEAK_TRACKER_HPP
