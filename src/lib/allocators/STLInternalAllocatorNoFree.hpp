/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef STL_INTERNAL_ALLOCATOR_NO_FREE_HPP
#define STL_INTERNAL_ALLOCATOR_NO_FREE_HPP

/********************  HEADERS  *********************/
#include <common/Debug.hpp>
#include "NoFreeAllocator.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  CLASS  **********************/
//Inspired from http://www.codeproject.com/Articles/4795/C-Standard-Allocator-An-Introduction-and-Implement
//Thanks to Lai Shiaw San Kent
template<typename T>
class STLInternalAllocatorNoFree {
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
			typedef STLInternalAllocatorNoFree<U> other;
		};

	public : 
		inline STLInternalAllocatorNoFree() {}
		inline ~STLInternalAllocatorNoFree() {}
		inline STLInternalAllocatorNoFree(STLInternalAllocatorNoFree<T> const&) {}
		template<typename U>
		inline STLInternalAllocatorNoFree(STLInternalAllocatorNoFree<U> const&) {}

		//    address
		inline pointer address(reference r) { return &r; }
		inline const_pointer address(const_reference r) { return &r; }

		//    memory allocation
		inline pointer allocate(size_type cnt, typename std::allocator<void>::const_pointer = 0) { 
			return reinterpret_cast<pointer>(gblNoFreeAllocator.allocate(cnt * sizeof(T))); 
		}
		inline void deallocate(pointer p, size_type) { 
			MATT_WARNING("Caution, you are using deallocate on STLAllocatorNoFree !");
			//gblInternaAlloc->free(p);
		}

		//    size
		inline size_type max_size() const { 
			return gblNoFreeAllocator.getMaxSize();
		}

		//    construction/destruction
		inline void construct(pointer p, const T& t) { new(p) T(t); }
		inline void destroy(pointer p) { p->~T(); }

		inline bool operator==(STLInternalAllocatorNoFree<T> const&) { return true; }
		inline bool operator!=(STLInternalAllocatorNoFree<T> const& a) { return !operator==(a); }
};    //    end of class Allocator

}

#endif //STL_INTERNAL_ALLOCATOR_NO_FREE_HPP
