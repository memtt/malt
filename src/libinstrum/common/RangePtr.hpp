/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.0
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/RangePtr.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_RANGE_PTR_HPP
#define MALT_RANGE_PTR_HPP

/**********************************************************/
#include <cstdlib>
#include <vector>
#include "STLInternalAllocator.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
class RangePtr
{
	public:
		RangePtr(void * base, void * end) {this->base = base; this->end = end;};
		RangePtr(void * base, size_t size) {this->base = base; this->end = (char*)this->base + size;};
		void set(void * base, void * end) {this->base = base; this->end = end;};
		void set(void * base, size_t size) {this->base = base; this->end = (char*)this->base + size;};
		bool contains(void * ptr) const {return ptr >= this->base && ptr < this->end;};
	private:
		void * base;
		void * end;
};

/**********************************************************/
class RangesPtr
{
	public:
		RangesPtr() {};
		void add(void * base, void * end) {ranges.emplace_back(base, end);};
		void add(void * base, size_t size) {ranges.emplace_back(base, size);};
		bool contains(void * ptr) const {for (auto & range : ranges) if (range.contains(ptr)) return true; return false;};
	private:
		std::vector<RangePtr, STLInternalAllocator<RangePtr> > ranges;
};

}

#endif //MALT_RANGE_PTR_HPP
