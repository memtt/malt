/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 09/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/CallStackInfo.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014 - 2022
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_CALL_STACK_INFO_HPP
#define MALT_CALL_STACK_INFO_HPP

/**********************************************************/
//standard
#include <ostream>
#include <cstdlib>
//from fftw (copied in extern-deps)
#include <portability/Clock.hpp>

/**********************************************************/
namespace htopml
{
	class JsonState;
}

/**********************************************************/
namespace MALT
{

/**********************************************************/
enum MemDomain
{
	MEM_DOMAIN_CPU = 0,
	MEM_DOMAIN_GPU = 1,
	MEM_DOMAIN_COUNT = 2,
};

/**********************************************************/
/**
 * Provide a simple class to track some quantitiy over time by taking care of their
 * extremal values. By default it capture :
 *    - number of operations
 *    - min/max of the operation values
 *    - global sum of the operation values
 * 
 * @brief Track min/max/count/sum of a value associated to an operation.
**/
struct SimpleQuantityHistory
{
	SimpleQuantityHistory(void);
	void addEvent(ssize_t value);
	void push(const MALT::SimpleQuantityHistory& value);
	ssize_t getMean(void) const;
	size_t count;
	ssize_t min;
	ssize_t max;
	ssize_t sum;
};

/**********************************************************/
//pre-declaration to avoid head loop inclusion
struct CallStackInfoGlobals;
struct CallStackInfoRealloc;

/**********************************************************/
/**
 * @brief Structure to define the informations we tracked for each call site.
**/
struct CallStackInfo
{
	public:
		CallStackInfo(void);
		void onAllocEvent(size_t value,size_t peakId);
		void onFreeEvent(size_t value,size_t peakId);
		void onReallocEvent(size_t oldSize,size_t newSize);
		void onFreeLinkedMemory(size_t value,ticks lifetime,size_t peakId, MemDomain domain);
		void onMmap(ssize_t value,size_t peakId);
		void onMunmap(ssize_t value,size_t peakId,bool subMunmap);
		void onGpuAllocEvent(size_t value,size_t peakId);
		void onGpuFreeEvent(size_t value,size_t peakId);
		void merge(const MALT::CallStackInfo& info);
		void writeAsCallgrindEntry(int line, std::ostream & out) const;
		void writeAsCallgrindCallEntry(int line, std::ostream& out) const;
		static void writeCallgrindEventDef(std::ostream & out);
		void updatePeak(size_t peakId, MemDomain domain);
		bool hasRealloc(void) const;
		const SimpleQuantityHistory & getAllocInfo(void) const;
		const SimpleQuantityHistory & getFreeInfo(void) const;
		const SimpleQuantityHistory & getLifetime(void) const;
		size_t getPeak(MemDomain domain) const;
	public:
		friend void convertToJson(htopml::JsonState& json, const CallStackInfo& value);
		friend std::ostream & operator << (std::ostream & out,const CallStackInfo & info);
		friend void convert(CallStackInfoGlobals & out,const CallStackInfo & in);
		friend void convert(CallStackInfoRealloc & out,const CallStackInfo & in);
	private:
		/** Track the min/max/sum/count of each memory allocation sizes. **/
		SimpleQuantityHistory alloc;
		/** Track the min/max/sum/count of each memory deallocation sizes. **/
		SimpleQuantityHistory free;
		/** Track the min/max/sum/count of each chunk lifetime. **/
		SimpleQuantityHistory lifetime;
		/** Track the min/max/sum/count of each memory allocation sizes from mmap. **/
		SimpleQuantityHistory mmap;
		/** Track the min/max/sum/count of each memory allocation sizes from mmap. **/
		SimpleQuantityHistory munmap;
		/** Track the min/max/sum/count of each memory allocation sizes from mmap. **/
		SimpleQuantityHistory gpuAlloc;
		/** Track the min/max/sum/count of each memory allocation sizes from mmap. **/
		SimpleQuantityHistory gpuFree;
		/** Count number of null size allocations. **/
		ssize_t cntZeros;
		/** Count the current allocated (alive) memory from this call site. **/
		ssize_t alive[MEM_DOMAIN_COUNT];
		/** Keep track of the maximum alive chunks during execution. **/
		ssize_t maxAlive[MEM_DOMAIN_COUNT];
		/** Keep track of the memory used on global application peak. **/
		ssize_t peak[MEM_DOMAIN_COUNT];
		/** Remember when we updated the peak prameter for the last time. **/
		size_t peakId[MEM_DOMAIN_COUNT];
		/** Keep track of the number of realloc. **/
		size_t reallocCount;
		/** Sum the memory allocated due to realloc. **/
		size_t reallocDelta;
};

/**********************************************************/
void convertToJson(htopml::JsonState& json, const SimpleQuantityHistory& value);

}

#endif //ATT_CALL_STACK_INFO_HPP
