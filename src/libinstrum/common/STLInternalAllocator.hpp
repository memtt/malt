/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/STLInternalAllocator.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef STL_INTERNAL_ALLOCATOR_HPP
#define STL_INTERNAL_ALLOCATOR_HPP

/**********************************************************/
#include "SimpleAllocator.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * Inspired from http://www.codeproject.com/Articles/4795/C-Standard-Allocator-An-Introduction-and-Implement
 * Thanks to Lai Shiaw San Kent
 * 
 * @brief STL allocator to redirect STL map/list onto our own internal allocator.
**/
template<typename T>
class STLInternalAllocator {
	public : 
		//    typedefs
		typedef T value_type;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef std::size_t size_type;
		typedef std::ptrdiff_t difference_type;

	public : 
		//    convert an allocator<T> to allocator<U>
		template<typename U>
		struct rebind {
			typedef STLInternalAllocator<U> other;
		};

	public : 
		inline STLInternalAllocator() {}
		inline ~STLInternalAllocator() {}
		inline STLInternalAllocator(STLInternalAllocator<T> const&) {}
		template<typename U>
		inline STLInternalAllocator(STLInternalAllocator<U> const&) {}

		//    address
		inline pointer address(reference r) { return &r; }
		inline const_pointer address(const_reference r) { return &r; }

		//    memory allocation
		inline pointer allocate(size_type cnt, typename std::allocator<void>::const_pointer = 0) { 
		return reinterpret_cast<pointer>(MALT_MALLOC(cnt * sizeof(T))); 
		}
		inline void deallocate(pointer p, size_type) { 
			MALT_FREE(p);
		}

		//    size
		inline size_type max_size() const { 
			return gblInternaAlloc->getMaxSize();
		}

		//    construction/destruction
		inline void construct(pointer p, const T& t) { new(p) T(t); }
		inline void destroy(pointer p) { p->~T(); }

		inline bool operator==(STLInternalAllocator<T> const&) const { return true; }
		inline bool operator!=(STLInternalAllocator<T> const& a) const { return !operator==(a); }
};    //    end of class Allocator

}

#endif //STL_INTERNAL_ALLOCATOR_HPP
