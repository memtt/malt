/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.0
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
class InternalLeakTracker
{
	public:
		InternalLeakTracker(bool track = false) {
			this->track = track;
		}
		~InternalLeakTracker(void) {};
		void onMalloc(void * ptr, size_t size)
		{
			if (!enabled)
				return;
			gblLocalCnt++;
			gblLocalMem+=size;
			if (gblLocalCnt % 1000 == 0)
				fprintf(stderr, "MALT: internal stat : %zu : %g MB (%zu)\n", gblLocalCnt.load(), (double)gblLocalMem.load()/1024.0/1024.0, size);
			gblBlockSizes[ptr] = size;
			stack.loadCurrentStack();
			gblBlockStack.emplace(ptr, stack);
			this->dump();
		}
		void onFree(void * ptr)
		{
			if (!enabled)
				return;
			size_t size = gblBlockSizes[ptr];
			gblLocalCnt--;
			gblLocalMem-=size;
			gblBlockSizes.erase(ptr);
			gblBlockStack.erase(ptr);
			if (gblLocalCnt % 1000 == 0)
				fprintf(stderr, "MALT: internal stat : %zu : %g MB (%zu)\n", gblLocalCnt.load(), (double)gblLocalMem.load()/1024.0/1024.0, size);
			this->dump();
		}
		void onRealloc(void * ptr, size_t size)
		{
			if (!enabled)
				return;
			gblLocalCnt+=0;
			gblLocalMem-=gblBlockSizes[ptr];
			gblBlockSizes.erase(ptr);
			if (size == 0)
				gblLocalCnt--;
			if (ptr == nullptr)
				gblLocalCnt++;
			if (gblLocalCnt % 1000 == 0)
				fprintf(stderr, "MALT: internal stat : %zu : %g MB (%zu)\n", gblLocalCnt.load(), (double)gblLocalMem.load()/1024.0/1024.0, size);
			gblBlockSizes[ptr] = size;
			stack.loadCurrentStack();
			gblBlockStack.emplace(ptr, stack);
			this->dump();
		}
		void dump(void)
		{
			if (this->gblLocalMem > 2000UL*1024UL*1204UL || this->gblLocalCnt > 10000) {
				SymbolSolver solver;
				solver.loadProcMap();
				for (const auto & it : this->gblBlockStack)
					it.second.solveSymbols(solver);
				solver.solveNames();
				/*for (const auto & it : this->gblBlockStack)
					it.second.printSymbols(solver);*/
				std::set<String, std::less<String>, MALT::STLInternalAllocator<String> > reducer;
				for (const auto & it : this->gblBlockStack)
					reducer.insert(it.second.toDebugString(solver));
				for (const auto & it : reducer)
					fprintf(stderr, "%s", it.c_str());
				abort();
			}
		}
	private:
		const bool enabled = false;
		bool track{false};
		std::atomic<size_t> gblLocalMem{0};
		std::atomic<size_t> gblLocalCnt{0};
		std::map<void*, size_t, std::less<void*>, MALT::STLInternalAllocator<std::pair<void*, size_t> > > gblBlockSizes;
		std::map<void*, Stack, std::less<void*>, MALT::STLInternalAllocator<std::pair<void*, Stack> > > gblBlockStack;
		BacktraceStack stack;
};

}

#endif //MALT_INTERNAL_LEAK_TRACKER_HPP
