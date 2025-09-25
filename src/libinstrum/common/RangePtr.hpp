/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
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
/**
 * @brief Define a rang of address.
 * 
 * It helps checking quickly if we are in side some ranges mostly to check
 * if we are inside some part of a library mapping in use space.
 */
class RangePtr
{
	public:
		/**
		 * @brief Construct a new Range Ptr object
		 * 
		 * @param base Base addresse.
		 * @param end End address (excluded).
		 */
		RangePtr(void * base, void * end) {this->base = base; this->end = end;};
		/**
		 * @brief Construct a new Range Ptr object
		 * 
		 * @param base Base address.
		 * @param size Size of the segment.
		 */
		RangePtr(void * base, size_t size) {this->base = base; this->end = (char*)this->base + size;};
		/**
		 * @brief Set the addresses of the range.
		 * 
		 * @param base Base addresse.
		 * @param end End address (excluded).
		 */
		void set(void * base, void * end) {this->base = base; this->end = end;};
		/**
		 * @brief Set the addresses of the range.
		 * 
		 * @param base Base address.
		 * @param size Size of the segment.
		 */
		void set(void * base, size_t size) {this->base = base; this->end = (char*)this->base + size;};
		/**
		 * @brief Check if the range contains the given address.
		 * 
		 * @param ptr Address to be checked.
		 * @return true If is in the range.
		 * @return false If not in the range.
		 */
		bool contains(void * ptr) const {return ptr >= this->base && ptr < this->end;};
	private:
		void * base;
		void * end;
};

/**********************************************************/
/**
 * @brief Define a set of address ranges.
 * 
 * If helps checking quickly if we are in one of the part of the memory mapping
 * of a library (object, global vars...).
 */
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
